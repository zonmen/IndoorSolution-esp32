#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "driver/gpio.h"
#include "wifi.h"


void app_main(void)
{
	//start wifi
	wifi_start();

}
