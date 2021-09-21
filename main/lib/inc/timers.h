#ifndef TIMER_HEADERFILE_H
#define TIMER_HEADERFILE_H


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/timer.h"
#include "time.h"
#include "sys/time.h"
#include "string.h"
#include "esp_log.h"

#define TIMER_DIVIDER         (16)  //  Hardware timer clock divider
#define TIMER_SCALE           (APB_CLK_FREQ / TIMER_DIVIDER)  // convert counter value to seconds


int debug_timer_counter_bl;
int debug_timer_counter_http_request;
int debug_timer_counter_measuring;
extern int flag_bl_send;
extern int flag_http_reuqest_send;
extern int flag_measuring;


struct Timer{
	int timer_group;
	int timer_idx;
	char* purpose;
};

static bool IRAM_ATTR timer_callback_function_bl(void *args);

static bool IRAM_ATTR timer_callback_function_http(void *args);

static bool IRAM_ATTR timer_callback_function_measuring(void *args);

void clock_init(struct Timer* timer_t);

void clock_start(struct Timer* timer_t);

void clock_stop(struct Timer* timer_t);

void clock_set_time(struct Timer* timer_t, int time_interval_sec);

int clock_get_time(struct Timer* timer_t);

#endif
