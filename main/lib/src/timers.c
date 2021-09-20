#include "timers.h"

int debug_timer_counter_bl = 0;
int debug_timer_counter_http_request = 0;
int debug_timer_counter_measuring = 0;

static bool IRAM_ATTR timer_callback_function_bl(void *args)
{
	debug_timer_counter_bl++;
	flag_bl_send = 1;
	//to detect if we need to use YIELD
	return pdTRUE;
}

static bool IRAM_ATTR timer_callback_function_http(void *args)
{
	debug_timer_counter_http_request++;
	flag_http_reuqest_send = 1;
	//to detect if we need to use YIELD
	return pdTRUE;
}

static bool IRAM_ATTR timer_callback_function_measuring(void *args)
{
	debug_timer_counter_measuring++;
	flag_measuring = 1;
	//to detect if we need to use YIELD
	return pdTRUE;
}

void clock_init(struct Timer* timer_t)
{
	int time_interval_sec = 5;
	//set timer configuration
	timer_config_t config = {
	            .divider = TIMER_DIVIDER,
	            .counter_dir = TIMER_COUNT_UP,
	            .counter_en = TIMER_PAUSE,
	            .alarm_en = TIMER_ALARM_EN,
	            .auto_reload = TIMER_AUTORELOAD_EN,
	        };
	//init timer with configuration
	timer_init(timer_t->timer_group, timer_t->timer_idx, &config);
	//current timer value
	timer_set_counter_value(timer_t->timer_group, timer_t->timer_idx, 0);
	//set max timer value
	timer_set_alarm_value(timer_t->timer_group, timer_t->timer_idx, time_interval_sec * TIMER_SCALE);
	//enable interruption
	timer_enable_intr(timer_t->timer_group, timer_t->timer_idx);
	//set callback function for interruption
	if( strcmp(timer_t->purpose, "bluetooth") == 0 )
	{
		timer_isr_callback_add(timer_t->timer_group, timer_t->timer_idx, timer_callback_function_bl, 0, 0);
	} else
		if( strcmp(timer_t->purpose, "measuring") == 0 )
		{
			timer_isr_callback_add(timer_t->timer_group, timer_t->timer_idx, timer_callback_function_measuring, 0, 0);
		} else
		{
			timer_isr_callback_add(timer_t->timer_group, timer_t->timer_idx, timer_callback_function_http, 0, 0);
		}
}
//
void clock_start(struct Timer* timer_t){
	timer_enable_intr(timer_t->timer_group, timer_t->timer_idx);
	timer_start(timer_t->timer_group, timer_t->timer_idx);
}

void clock_stop(struct Timer* timer_t){
	timer_pause(timer_t->timer_group, timer_t->timer_idx);
	timer_disable_intr(timer_t->timer_group, timer_t->timer_idx);
}

void clock_set_time(struct Timer* timer_t, int time_interval_sec){
	timer_set_counter_value(timer_t->timer_group, timer_t->timer_idx, 0);
	timer_set_alarm_value(timer_t->timer_group, timer_t->timer_idx, time_interval_sec * TIMER_SCALE);
}

void clock_set_time_2(int timer_group, int timer_idx, int time_interval_sec){
	timer_set_counter_value(timer_group, timer_idx, 0);
	timer_set_alarm_value(timer_group, timer_idx, time_interval_sec * TIMER_SCALE);
}

int clock_get_time(struct Timer* timer_t){
	uint64_t counter_value = 0;
	int value_sec = 0;
	timer_get_alarm_value(timer_t->timer_group, timer_t->timer_idx, &counter_value);
	value_sec = (double) counter_value / TIMER_SCALE;
	return value_sec;
}
