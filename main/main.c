#include <stdio.h>
#include "driver/gpio.h"



void app_main(void)
{
	gpio_set_direction(2, GPIO_MODE_OUTPUT);
	gpio_set_level(2, 1);

}
