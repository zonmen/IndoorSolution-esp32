#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <string.h>
#include "driver/gpio.h"
#include "wifi.h"
#include "mhz19b.h"

const char* TAG = "main";

#include "ds3231.h"

#include "bme680.h"

#define DS3231_I2C_SDA_PIN 19
#define DS3231_I2C_SCL_PIN 21

#define BME680_SDA_PIN 22
#define BME680_SCL_PIN 23
#define I2C_PORT 0

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



void app_main(void)
{
	int mhz19b_co2 = 0;
	char time_string[30];

	//start wifi
	wifi_start();

	//ds3231
	ESP_ERROR_CHECK(i2cdev_init());
	i2c_dev_t dev;
	memset(&dev, 0, sizeof(i2c_dev_t));
	ESP_ERROR_CHECK(ds3231_init_desc(&dev, I2C_PORT, DS3231_I2C_SDA_PIN, DS3231_I2C_SCL_PIN));
	  //set time(GMT) to esp32 from ds3231(only after start)
	struct tm ds3231_time_tm;
	ds3231_get_time(&dev, &ds3231_time_tm);
	time_t temp_time_time_t = mktime(&ds3231_time_tm);
	struct timeval temp_time_timeval = { .tv_sec = temp_time_time_t };
	settimeofday(&temp_time_timeval, NULL);


	//bme680
	bme680_t bme680_sensor;
	memset(&bme680_sensor, 0, sizeof(bme680_t));
	ESP_ERROR_CHECK(bme680_init_desc(&bme680_sensor, BME680_I2C_ADDR_1, I2C_PORT, BME680_SDA_PIN, BME680_SCL_PIN));
	ESP_ERROR_CHECK(bme680_init_sensor(&bme680_sensor));
	uint32_t bme680_duration;
	bme680_get_measurement_duration(&bme680_sensor, &bme680_duration);
	bme680_values_float_t bme680_values;


	while(1){
		//measure data from sensors
		mhz19b_co2 = mhz19b_get_co2();
		bme680_get_data(bme680_sensor, bme680_duration, &bme680_values);
		//display data value
		ESP_LOGI(TAG, "co2 = %d", mhz19b_co2);
		ESP_LOGI(TAG, "temperature = %d", (int)bme680_values.temperature);
		ESP_LOGI(TAG, "pressure = %d", (int)bme680_values.pressure);
		ESP_LOGI(TAG, "humidity = %d\n", (int)bme680_values.humidity);
		//time delay between measuring
		vTaskDelay(5000 / portTICK_PERIOD_MS);
		}
}