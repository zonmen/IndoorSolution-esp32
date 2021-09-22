#ifndef MHZ19B_HEADERFILE_H
#define MHZ19B_HEADERFILE_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "esp_log.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "driver/gpio.h"
#include "driver/uart.h"

#define ECHO_TEST_TXD_2  (GPIO_NUM_17)
#define ECHO_TEST_RXD_2  (GPIO_NUM_16)
#define ECHO_TEST_RTS  (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS  (UART_PIN_NO_CHANGE)
#define BUF_SIZE (1024)

extern int flag_status_mhz19b;

int mhz19b_config();

int mhz19b_detect();

int mhz19b_get_co2();

void mhz19b_set_auto_callibration(int value);

#endif
