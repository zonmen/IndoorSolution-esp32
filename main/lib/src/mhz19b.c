#include "mhz19b.h"

void mhz19b_config(){
	uart_config_t uart_config_2 = {
		        .baud_rate = 9600,
		        .data_bits = UART_DATA_8_BITS,
		        .parity    = UART_PARITY_DISABLE,
		        .stop_bits = UART_STOP_BITS_1,
		        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		        .source_clk = UART_SCLK_APB,
		    };
	uart_driver_install(UART_NUM_2, BUF_SIZE * 2, 0, 0, NULL, 0);
	uart_param_config(UART_NUM_2, &uart_config_2);
	uart_set_pin(UART_NUM_2, ECHO_TEST_TXD_2, ECHO_TEST_RXD_2, ECHO_TEST_RTS, ECHO_TEST_CTS);
}

int mhz19b_get_co2(){
	int result = -1;
    const char data_read[9];
	//command to get co2 concentration
    char  data_write[9] = {0xff, 0x01, 0x86, 0x00,
    		0x00,0x00,0x00,0x00, 0x79};
	uart_write_bytes(UART_NUM_2, data_write, 9);
	uart_read_bytes(UART_NUM_2, data_read, 9, 100);
	result = data_read[2]* 256 + data_read[3];

	return result;
}

void mhz19b_auto_callibration(){
	char  data_write_calibration[9] = {0xff, 0x01, 0x79, 0xa0,
	         		0x00,0x00,0x00,0x00, 0xe6};
	uart_write_bytes(UART_NUM_2, data_write_calibration	, 9);
}
