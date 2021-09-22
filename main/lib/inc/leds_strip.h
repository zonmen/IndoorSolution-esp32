#ifndef LEDS_STRIPE_HEADERFILE_H
#define LEDS_STRIPE_HEADERFILE_H

#include <stdio.h>
#include <string.h>
#include <esp_system.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_MAX_DUTY               (8189) // Set duty to 100%. duty resolution is 13 bits, so 100% - (2 ** 13) - 1 = 8189
#define LEDC_FREQUENCY          (5000) // Frequency in Hertz. Set frequency at 5 kHz

#define LEDC_CHANNEL_RED            LEDC_CHANNEL_0
#define LEDC_CHANNEL_GREEN          LEDC_CHANNEL_1
#define LEDC_CHANNEL_BLUE           LEDC_CHANNEL_2
#define LEDC_PIN_RED            (27)
#define LEDC_PIN_GREEN          (26)
#define LEDC_PIN_BLUE	        (25)


int red_portion;
int green_portion;
int blue_portion;
extern int flag_led_indication;
extern int flag_blink;

void leds_strip_init();

void leds_strip_refresh();

void leds_strip_indication(int co2);
#endif
