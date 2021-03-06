#include <leds_strip.h>
#include <leds_strip.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <string.h>
#include "driver/gpio.h"

#include "wifi.h"
#include "mhz19b.h"
#include "ds3231.h"
#include "bme680.h"
#include "http_request.h"
#include "bluetooth.h"
#include "leds_strip.h"
#include "timers.h"

const char* TAG = "main";
int flag_server_request_enable = 1;
int flag_led_indication = 1;
int flag_bl_connect = 0;
int flag_http_reuqest_send = 0;
int flag_measuring = 0;
int flag_bl_send = 0;
//status
int flag_status_wifi_connected = 0;
int flag_status_http_sent = 0;
int flag_status_mhz19b = 0;
int flag_status_bme680 = 0;
int flag_status_ds3231 = 0;

#define DS3231_I2C_SDA_PIN 19
#define DS3231_I2C_SCL_PIN 21

#define BME680_SDA_PIN 22
#define BME680_SCL_PIN 23
#define I2C_PORT 0

struct Timer http_request_timer = {
		.timer_group = 1,
		.timer_idx = 0,
		.purpose = "http_request"
};
struct Timer measuring_timer = {
		.timer_group = 0,
		.timer_idx = 1,
		.purpose = "measuring"
};

void bme680_get_data(bme680_t sensor, uint32_t duration, bme680_values_float_t* values){
	if (bme680_force_measurement(&sensor) == ESP_OK) // STEP 1
	{
	    // STEP 2: busy waiting until measurement results are available
	    bool busy;
	    do
	    {
	        if (bme680_is_measuring(&sensor, &busy) != ESP_OK)
	            break;
	    }
	    while (busy);

	    // STEP 3: get the results and do something with them
	    bme680_get_results_float (&sensor, values);
	}
}

void get_time_string(char* line){

	char temp_line[30];
	char string_value[15];
	//get time from esp32
	struct timeval temp_time_timeval;
	gettimeofday(&temp_time_timeval, NULL);
	time_t temp_time_time_t = temp_time_timeval.tv_sec;
	struct tm esp32_time_tm;
	localtime_r(&temp_time_time_t, &esp32_time_tm);
	//create time string in right format
	sprintf(string_value, "%d",esp32_time_tm.tm_year+1900);
	strcpy(temp_line,string_value);
	strcat(temp_line, "-");
	sprintf(string_value, "%d",esp32_time_tm.tm_mon+1);
	strcat(temp_line,string_value);
	strcat(temp_line, "-");
	sprintf(string_value, "%d",esp32_time_tm.tm_mday);
	strcat(temp_line,string_value);
	strcat(temp_line, "T");
	sprintf(string_value, "%d",esp32_time_tm.tm_hour);
	strcat(temp_line,string_value);
	strcat(temp_line, ":");
	sprintf(string_value, "%d",esp32_time_tm.tm_min);
	strcat(temp_line,string_value);
	strcat(temp_line, ":");
	sprintf(string_value, "%d",esp32_time_tm.tm_sec);
	strcat(temp_line,string_value);
	strcat(temp_line, "Z");

	strcpy(line, temp_line);
}

void app_main(void)
{
	int mhz19b_co2 = 0, co2_sum = 0, co2_counter = 0;
	int counter = 0;
	char time_string[30];
	//start wifi
	wifi_start();

	//bluetooth
	bluetooth_init();

	//mhz19b
	if( mhz19b_config() != 1 ){
		ESP_LOGE("MAIN", "Sensor MHZ19B is not detected");
	} else ESP_LOGI("MAIN", "Sensor MHZ19B is OK");
	mhz19b_set_auto_callibration(0);

	//led strip
	leds_strip_init();

	//ds3231
	ESP_ERROR_CHECK(i2cdev_init());
	i2c_dev_t dev;
	memset(&dev, 0, sizeof(i2c_dev_t));
	while(ds3231_init_desc(&dev, I2C_PORT, DS3231_I2C_SDA_PIN, DS3231_I2C_SCL_PIN) != ESP_OK){
		counter++;
		if(counter == 6){
			break;
		}
	}
	if(counter == 6){
		ESP_LOGE("MAIN", "Sensor DS3231 is not detected");
		flag_status_ds3231 = -1;
	} else{
		ESP_LOGI("MAIN", "Sensor DS3231 is OK");
		flag_status_ds3231 = 1;

	}
		//set time(GMT) to esp32 from ds3231(only after start)
	if(flag_status_ds3231 == 1){
		struct tm ds3231_time_tm;
		ds3231_get_time(&dev, &ds3231_time_tm);
		time_t temp_time_time_t = mktime(&ds3231_time_tm);
		struct timeval temp_time_timeval = { .tv_sec = temp_time_time_t };
		settimeofday(&temp_time_timeval, NULL);
		get_time_string(time_string);
	} else{
		strcpy(time_string, "1111-1-1T11:11:11Z");
	}

	//bme680
	bme680_t bme680_sensor;
	memset(&bme680_sensor, 0, sizeof(bme680_t));
	ESP_ERROR_CHECK(bme680_init_desc(&bme680_sensor, BME680_I2C_ADDR_1, I2C_PORT, BME680_SDA_PIN, BME680_SCL_PIN));
	while(bme680_init_sensor(&bme680_sensor) != ESP_OK){
		counter++;
		if(counter == 6){
			break;
		}
	}
	if(counter == 6){
		ESP_LOGE("MAIN", "Sensor BME680 is not detected");
		flag_status_bme680 = -1;
	} else{
		ESP_LOGI("MAIN", "Sensor BME680 is OK");
		flag_status_bme680 = 1;
	}
	uint32_t bme680_duration;
	bme680_get_measurement_duration(&bme680_sensor, &bme680_duration);
	bme680_values_float_t bme680_values = {
			.temperature = 0,
		    .pressure = 0,
		    .humidity = 0
	};

	//Init and start timers for http request and to trigger measuring data
	clock_init(&http_request_timer);
	clock_set_time(&http_request_timer, 20 * 60);
	clock_start(&http_request_timer);

	clock_init(&measuring_timer);
	clock_set_time(&measuring_timer, 10 * 1);
	clock_start(&measuring_timer);

	while(1){
		//measure and display data from sensors
		if(flag_measuring == 1){
			if(flag_status_mhz19b == 1){
				mhz19b_co2 = mhz19b_get_co2();
				co2_sum += mhz19b_co2;
				co2_counter ++;
			}else{
				mhz19b_co2 = -100;
				if( mhz19b_detect() != 1 ){
					ESP_LOGE("MAIN", "Sensor MHZ19B is not detected");
				} else ESP_LOGI("MAIN", "Sensor MHZ19B is OK");
			}
			if(flag_status_bme680 == 1){
				bme680_get_data(bme680_sensor, bme680_duration, &bme680_values);
			} else{
				bme680_values.temperature = -100;
				bme680_values.pressure = -100;
				bme680_values.humidity = -100;
			}
			flag_measuring = 0;
			//display data value
			ESP_LOGI("MAIN(measure data)", "%s", time_string);
			ESP_LOGI("MAIN(measure data)", "co2 = %d", mhz19b_co2);
			ESP_LOGI("MAIN(measure data)", "temperature = %d", (int)bme680_values.temperature);
			ESP_LOGI("MAIN(measure data)", "pressure = %d", (int)bme680_values.pressure);
			ESP_LOGI("MAIN(measure data)", "humidity = %d\n", (int)bme680_values.humidity);
			//send data via bluetooth
			if(flag_bl_connect == 1){
				if(flag_status_ds3231 == 1){
					get_time_string(time_string);
				}
				write_data(time_string, 0.0, 0.0, bme680_values.temperature,
					bme680_values.pressure, bme680_values.humidity, mhz19b_co2, 0, 0);
			}
		}
		//update led strip color
		leds_strip_indication(mhz19b_co2);
		//send data to server
		if(flag_http_reuqest_send == 1 && flag_server_request_enable == 1 && flag_status_wifi_connected == 1){
			if(flag_status_ds3231 == 1){
				get_time_string(time_string);
			}
			if( co2_counter == 0){
				co2_counter = 1;
			}
			https_post_request("d1", 0, 0, time_string, (co2_sum / co2_counter),
			bme680_values.temperature, bme680_values.pressure,
			bme680_values.humidity);
			co2_sum = 0;
			co2_counter = 0;
			flag_http_reuqest_send = 0;
			ESP_LOGW("DEBUG main", "end http request");
		}
		//time delay in main loop
		vTaskDelay(100 / portTICK_PERIOD_MS);
		}
}
