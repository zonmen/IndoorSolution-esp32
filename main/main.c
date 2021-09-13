#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "driver/gpio.h"
#include "wifi.h"
#include "mhz19b.h"

const char* TAG = "main";

void app_main(void)
{
	int mhz19b_co2 = 0;

	//start wifi
	wifi_start();
	while(1){
		//measure co2 from mhz19b sensor
		mhz19b_co2 = mhz19b_get_co2();
		//display co2 value in console
		ESP_LOGI("DEBUG", "co2 = %d", mhz19b_co2);
		//time delay between measuring
		vTaskDelay(5000 / portTICK_PERIOD_MS);
		}
}
