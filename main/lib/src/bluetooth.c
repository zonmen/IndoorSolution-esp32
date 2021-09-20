#include "bluetooth.h"




void write_word_and_int(char* word, int value){
//	char line[50];
//	strcat(line, "co2 = ");
//	strcat(line, "53");
//	uint8_t length = strlen(line);
//	uint8_t* data = (uint8_t *) line;
//	//	ESP_LOGI("DEBUG", "almost finish write function");
//	esp_spp_write(write_handle, length, data);


	char line[50];
	char string_value[15];
	strcpy(line, word);
	sprintf(string_value, "%d", value);
	strcat(line, string_value);
	strcat(line, "\n");
	uint8_t length = strlen(line);
	uint8_t* data = (uint8_t *) line;
	esp_spp_write(write_handle, length, data);
}

void write_data(char* probe_datatime, float longitude, float latitude,
		int temperature, int presure, int humidity, int co2, int status, int response)
{
	//create line to send via bluetooth
	char line[150];
	char value_string[30];
	strcpy(line, probe_datatime);

	strcat(line, " ");
	sprintf(value_string, "%f", latitude);
	strcat(line, value_string);

	strcat(line, " ");
	sprintf(value_string, "%f", longitude);
	strcat(line, value_string);

	strcat(line, " ");
	sprintf(value_string, "%d", temperature);
	strcat(line, value_string);

	strcat(line, " ");
	sprintf(value_string, "%d", presure);
	strcat(line, value_string);

	strcat(line, " ");
	sprintf(value_string, "%d", humidity);
	strcat(line, value_string);

	strcat(line, " ");
	sprintf(value_string, "%d", co2);
	strcat(line, value_string);

	strcat(line, " ");
	sprintf(value_string, "%d", status);
	strcat(line, value_string);

	strcat(line, " ");
	sprintf(value_string, "%d", response);
	strcat(line, value_string);

	strcat(line, "\n");

	//send data
	uint8_t length = strlen(line);
	uint8_t* data = (uint8_t *) line;
	esp_spp_write(write_handle, length, data);
}


void command_set_time_bluetooth_data(char* command){
	int command_data_place = 11;
	int time_interval = 0;
	time_interval = strtol((command + command_data_place * sizeof(char)), NULL, 10);
	if(time_interval >= 5){
		ESP_LOGI("DEBUG_bluetooth.c", "bluetooth change time");
		clock_set_time(&bluetooth_timer, time_interval);
	}
}

void command_set_time_server_data(char* command){
	int command_data_place = 10;
	int time_interval = 0;
	time_interval = strtol((command + command_data_place * sizeof(char)), NULL, 10);
	if(time_interval >= 5){
		ESP_LOGI("DEBUG_bluetooth.c", "server change time");
		clock_set_time(&http_request_timer, time_interval);
	}
}
//TIME_M=5
void command_set_time_measure_data(char* command){
	int command_data_place = 10;
	int time_interval = 0;
	time_interval = strtol((command + command_data_place * sizeof(char)), NULL, 10);
	if(time_interval >= 5){
		ESP_LOGI("DEBUG_bluetooth.c", "measuring change time");
		clock_set_time(&measuring_timer, time_interval);
	}
}

void bluetooth_command_led_indication_enable(char* command){
	int command_data_place = 7;
	int value = 0;
	value = strtol((command + command_data_place * sizeof(char)), NULL, 10);
	if(value == 0 || value == 1 || value == 2){
		flag_led_indication = value;
	} else{
		flag_led_indication = 1;
	}
}

void bluetooth_command_server_request_enable(char* command){
	int command_data_place = 10;
	int value = 0;
	value = strtol((command + command_data_place * sizeof(char)), NULL, 10);
	if(value != 0){
		flag_server_request = 1;
	} else{
		flag_server_request = 0;
	}
}



static void read_data(uint8_t* data_read, uint16_t length){
//determine command
	char* line = (char *)data_read;
	// command = TIMEBAT (set time in seconds between measurements)
	if(line[3] == 'T' && line[4] == 'I' && line[5] == 'M' && line[6] == 'E'
				&& line[7] == 'B' && line[8] == 'A' && line[9] == 'T' && line[10] == '='){
			printf("TIMEBAT=");
			command_set_time_bluetooth_data(line);
		}
		// command = TIME_S (time to send data to server
		else if(line[3] == 'T' && line[4] == 'I' && line[5] == 'M' && line[6] == 'E'
				&& line[7] == '_' && line[8] == 'S'){
			printf("TIME_S=");
			command_set_time_server_data(line);
		}
		// command = TIME_M(time between sensors measuring, min = 5 sec)
		else if(line[3] == 'T' && line[4] == 'I' && line[5] == 'M' && line[6] == 'E'
				&& line[7] == '_' && line[8] == 'M'){
			printf("TIME_M=");
			command_set_time_measure_data(line);
		}
		// command = LED on(1)/off(0)
		else if(line[3] == 'L' && line[4] == 'E' && line[5] == 'D'){
			printf("LED");
			bluetooth_command_led_indication_enable(line);
		}
		// command = SERVER REQUEST on(1)/off(0)
		else if(line[3] == 'S' && line[4] == 'E' && line[5] == 'R' && line[6] == 'V'
				&& line[7] == 'E' && line[8] == 'R'){
			printf("SERVER");
			bluetooth_command_server_request_enable(line);
		}
		// command = CONNECT (return id and phone number)
		else if(line[3] == 'C' && line[4] == 'O' && line[5] == 'N' && line[6] == 'N'
				&& line[7] == 'E' && line[8] == 'C' && line[9] == 'T'){
			printf("CONNECT");
		}
		// command = LONG (set longitude (must be 7 position))
		else if(line[3] == 'L' && line[4] == 'O' && line[5] == 'N' && line[6] == 'G'
				&& line[7] == '='){
			printf("LONG");
		}
		//commnad = LAT (set lotittude (must be 7 position))
		else if(line[3] == 'L' && line[4] == 'A' && line[5] == 'T' && line[6] == '='){
				printf("LAT");
			}
		// command = ID (set id number)
		else if(line[3] == 'I' && line[4] == 'D' && line[5] == '='){
					printf("id");
				}
		// command = DATA (set time and data (must be 2 position for each set))
		else if(line[3] == 'D' && line[4] == 'A' && line[5] == 'T' && line[6] == 'A'
				&& line[7] == '='){
					printf("data");
				}

}

static void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    switch (event) {
    case ESP_SPP_INIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_INIT_EVT");
        esp_bt_dev_set_device_name(EXAMPLE_DEVICE_NAME);
        esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
        esp_spp_start_srv(sec_mask,role_slave, 0, SPP_SERVER_NAME);
        break;
    case ESP_SPP_DISCOVERY_COMP_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_DISCOVERY_COMP_EVT");
        break;
    case ESP_SPP_OPEN_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_OPEN_EVT, handle = %d", param->open.handle);
        break;
    case ESP_SPP_CLOSE_EVT:			//when we disconnected (from console)
        ESP_LOGI(SPP_TAG, "ESP_SPP_CLOSE_EVT");
        clock_stop(&bluetooth_timer);
        break;
    case ESP_SPP_START_EVT:			//when start
        ESP_LOGI(SPP_TAG, "ESP_SPP_START_EVT, handle = %d", param->open.handle);
        clock_init(&bluetooth_timer);
        break;
    case ESP_SPP_CL_INIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CL_INIT_EVT");
        break;
    case ESP_SPP_DATA_IND_EVT:			//when receive data
    	read_data(param->data_ind.data, param->data_ind.len);

    	break;
    case ESP_SPP_CONG_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CONG_EVT");
        break;
    case ESP_SPP_WRITE_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_WRITE_EVT");
        break;
    case ESP_SPP_SRV_OPEN_EVT:			//when connect (with console)
        ESP_LOGI(SPP_TAG, "ESP_SPP_SRV_OPEN_EVT, handle = %d", param->open.handle);
        write_handle = param->open.handle;
        clock_start(&bluetooth_timer);
        break;
    case ESP_SPP_SRV_STOP_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_SRV_STOP_EVT");
        break;
    case ESP_SPP_UNINIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_UNINIT_EVT");
        break;
    default:
        break;
    }
}

void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
    switch (event) {
    case ESP_BT_GAP_AUTH_CMPL_EVT:{
        if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {
            ESP_LOGI(SPP_TAG, "authentication success: %s", param->auth_cmpl.device_name);
            esp_log_buffer_hex(SPP_TAG, param->auth_cmpl.bda, ESP_BD_ADDR_LEN);
        } else {
            ESP_LOGE(SPP_TAG, "authentication failed, status:%d", param->auth_cmpl.stat);
        }
        break;
    }
    case ESP_BT_GAP_PIN_REQ_EVT:{
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_PIN_REQ_EVT min_16_digit:%d", param->pin_req.min_16_digit);
        if (param->pin_req.min_16_digit) {
            ESP_LOGI(SPP_TAG, "Input pin code: 0000 0000 0000 0000");
            esp_bt_pin_code_t pin_code = {0};
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 16, pin_code);
        } else {
            ESP_LOGI(SPP_TAG, "Input pin code: 1234");
            esp_bt_pin_code_t pin_code;
            pin_code[0] = '1';
            pin_code[1] = '2';
            pin_code[2] = '3';
            pin_code[3] = '4';
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
        }
        break;
    }

#if (CONFIG_BT_SSP_ENABLED == true)
    case ESP_BT_GAP_CFM_REQ_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_CFM_REQ_EVT Please compare the numeric value: %d", param->cfm_req.num_val);
        esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
        break;
    case ESP_BT_GAP_KEY_NOTIF_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_KEY_NOTIF_EVT passkey:%d", param->key_notif.passkey);
        break;
    case ESP_BT_GAP_KEY_REQ_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_KEY_REQ_EVT Please enter passkey!");
        break;
#endif

    case ESP_BT_GAP_MODE_CHG_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_MODE_CHG_EVT mode:%d", param->mode_chg.mode);
        break;

    default: {
        ESP_LOGI(SPP_TAG, "event: %d", event);
        break;
    }
    }
    return;
}

void bluetooth_init()
{
	//initialize global variables
	write_handle = 0;
	flag_bl_send = 0;


    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    if ((ret = esp_bt_controller_init(&bt_cfg)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s initialize controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bluedroid_init()) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s initialize bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bluedroid_enable()) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s enable bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bt_gap_register_callback(esp_bt_gap_cb)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s gap register failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_spp_register_callback(esp_spp_cb)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s spp register failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_spp_init(esp_spp_mode)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s spp init failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

#if (CONFIG_BT_SSP_ENABLED == true)
    /* Set default parameters for Secure Simple Pairing */
    esp_bt_sp_param_t param_type = ESP_BT_SP_IOCAP_MODE;
    esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_IO;
    esp_bt_gap_set_security_param(param_type, &iocap, sizeof(uint8_t));
#endif

    /*
     * Set default parameters for Legacy Pairing
     * Use variable pin, input pin code when pairing
     */
    esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_VARIABLE;
    esp_bt_pin_code_t pin_code;
    esp_bt_gap_set_pin(pin_type, 0, pin_code);



  //  timer_start(timer_group, timer_idx);

}
