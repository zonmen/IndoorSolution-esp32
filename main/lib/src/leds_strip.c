#include <leds_strip.h>

void leds_strip_init()
{
	flag_blink = 0;
	//initialize color coefficient
	red_portion = 1;
	green_portion = 1;
	blue_portion = 1;
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 5 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel[3] = {
    		{
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_RED,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_PIN_RED,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    		},
			{
			        .speed_mode     = LEDC_MODE,
			        .channel        = LEDC_CHANNEL_GREEN,
			        .timer_sel      = LEDC_TIMER,
			        .intr_type      = LEDC_INTR_DISABLE,
			        .gpio_num       = LEDC_PIN_GREEN,
			        .duty           = 0, // Set duty to 0%
			        .hpoint         = 0
			    		},
						{
						        .speed_mode     = LEDC_MODE,
						        .channel        = LEDC_CHANNEL_BLUE,
						        .timer_sel      = LEDC_TIMER,
						        .intr_type      = LEDC_INTR_DISABLE,
						        .gpio_num       = LEDC_PIN_BLUE,
						        .duty           = 0, // Set duty to 0%
						        .hpoint         = 0
						    		}
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel[0]));
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel[1]));
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel[2]));
}

void leds_strip_refresh(){
	ledc_stop(LEDC_MODE, LEDC_CHANNEL_RED, 0);
	ledc_stop(LEDC_MODE, LEDC_CHANNEL_GREEN, 0);
	ledc_stop(LEDC_MODE, LEDC_CHANNEL_BLUE, 0);

}

void leds_strip_indication(int co2){
	leds_strip_refresh();

	if(flag_led_indication == 0);

	if(flag_led_indication == 1)
	{
	if(co2 < 1000 && co2 > 400 && co2 < 5000){
		//light green
		 ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_GREEN, LEDC_MAX_DUTY));
		 ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_GREEN));
	} else
		if(co2 >= 1000 && co2 < 1250 && co2 < 5000){
			//light orange(red + 0.3 * green)
			ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_RED, LEDC_MAX_DUTY));
			ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_RED));

			ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_GREEN, LEDC_MAX_DUTY * 0.3));
			ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_GREEN));
	} else
		if(co2 > 1250 && co2 < 2000 && co2 < 5000){
			//light orange(red + 0.1 * green)
			ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_RED, LEDC_MAX_DUTY));
			ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_RED));

			ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_GREEN, LEDC_MAX_DUTY * 0.1));
			ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_GREEN));

	} else
		if(co2 >= 2000 && co2 <= 3000 && co2 < 5000){
			//light red * 0.75
			ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_RED, LEDC_MAX_DUTY * 0.75));
			ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_RED));

	} else
		if(co2 > 3000 && co2 <= 5000){
			ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_RED, LEDC_MAX_DUTY));
			ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_RED));
		}
		//sensor is not ready - light blue
		else {
			flag_blink++;
			if(flag_blink <= 8){
				ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_RED, LEDC_MAX_DUTY * 0.2 ));
				ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_RED));

				ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_GREEN, LEDC_MAX_DUTY * 0.27));
				ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_GREEN));

				ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_BLUE, LEDC_MAX_DUTY  * 0.70));
				ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_BLUE));
			} else{
				ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_RED, LEDC_MAX_DUTY * 0.2 ));
				ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_RED));

				ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_GREEN, LEDC_MAX_DUTY * 0.27));
				ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_GREEN));

				ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_BLUE, LEDC_MAX_DUTY  * 0.51));
				ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_BLUE));
				if(flag_blink == 16) flag_blink = 0;
			}
		}
	}

	if(flag_led_indication == 2){
		ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_RED, LEDC_MAX_DUTY));
		ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_RED));

		ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_GREEN, LEDC_MAX_DUTY));
		ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_GREEN));

		ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_BLUE, LEDC_MAX_DUTY));
		ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_BLUE));
	}
	//any color
	if(flag_led_indication == 3){
			ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_RED, LEDC_MAX_DUTY * ((float)red_portion / 100)) );
			ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_RED));

			ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_GREEN, LEDC_MAX_DUTY * ((float)green_portion / 100)));
			ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_GREEN));

			ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_BLUE, LEDC_MAX_DUTY  * ((float)blue_portion / 100)));
			ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_BLUE));
		}
}
