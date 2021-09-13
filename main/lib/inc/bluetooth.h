#ifndef BLUETOOTH_HEADERFILE_H
#define BLUETOOTH_HEADERFILE_H

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"
#include "driver/gpio.h"
//for timer
#include "driver/timer.h"
#define TIMER_DIVIDER         (16)  //  Hardware timer clock divider
#define TIMER_SCALE           (APB_CLK_FREQ / TIMER_DIVIDER)  // convert counter value to seconds
#define timer_group TIMER_GROUP_0
#define timer_idx TIMER_0
#include "time.h"
#include "sys/time.h"

#define SPP_TAG "esp32_spp_tag"
#define SPP_SERVER_NAME "SPP_SERVER"
#define EXAMPLE_DEVICE_NAME "Indoor Solution 1"
#define SPP_SHOW_DATA 0
#define SPP_SHOW_SPEED 1
#define SPP_SHOW_MODE SPP_SHOW_SPEED    /*Choose show mode: show data or speed*/

static const esp_spp_mode_t esp_spp_mode = ESP_SPP_MODE_CB;
static const esp_spp_sec_t sec_mask = ESP_SPP_SEC_AUTHENTICATE;
static const esp_spp_role_t role_slave = ESP_SPP_ROLE_SLAVE;


int write_handle;
extern int flag_bl_send;
extern int flag_led_indication;
extern int flag_server_request;

//callback function for timer
static bool IRAM_ATTR timer_callback_function(void *args);

//initialize timer
void clock_init();

//start timer
void clock_start();

//stop timer
void clock_stop();

//set time to timer
void clock_set_time(int time_interval_sec);

//get time from timer
int clock_get_time();

//special format to send to bluetooth client
void write_word_and_int(char* word, int value);

//format and send data to bluetooth client
void write_data(char* probe_datatime, float longitude, float latitude,
		int temperature, int presure, int humidity, int co2, int status, int response);

//set time between sending data to bluetooth client
void bluetooth_command_send_time_set(char* command);


//value = 0, turn off led strip
//value = 1, turn on light co2 indication
//value = 2, turn on just white light
void bluetooth_command_led_indication_enable(char* command);

//enable(1)/disable(0) http request
void bluetooth_command_server_request_enable(char* command);

//read command from client
static void read_data(uint8_t* data_read, uint16_t length);

static void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);

void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param);

void bluetooth_init();

#endif
