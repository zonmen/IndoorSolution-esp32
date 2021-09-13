#include <http_request.h>

esp_err_t _http_event_handle(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI("http", "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI("http", "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI("http", "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI("http", "HTTP_EVENT_ON_HEADER");
            printf("%.*s", evt->data_len, (char*)evt->data);
            break;
        case HTTP_EVENT_ON_DATA:
            //ESP_LOGI("Denis", "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                printf("%.*s", evt->data_len, (char*)evt->data);
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI("http", "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI("http", "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}



void https_post_request(char* name, float latitude, float longitude, char* probe_datatime, int co, int temperature, int presure, int humidity)
{
	//whole request-example to server
	//char* request_data = "{\"access-token\":\"eyJ0eXAiOiJKV1QiLCJhbGciOiJSUzI1NiJ9.eyJ1c2VyX2lkIjoxMywic29pbCI6Ijg5YjIzNGEyNzI2ZDQ4ODI4YmQxYzJkMzdiM2Q2ZDk4In0.o2c-cG_DmFXqISyU0hFi95Spmy-HU2sU3ROHNKyNwdek6ykDSHmQY-1Ucekj6jElVAyYXBBlx5tU_iegSQ6g_mMl3aQH6nTEnKwpknsZ_T11X7-NoOcYUovqKSY1wy2N5m85S4N8aMQMblPng5ULPGhbkrlcYwN5ntL6DhSAZ5bbSQBj_amcdzFAD-wRpDIEkyJDglzVkSZRW1k2qaANtDOAI-GQiSZsuLepiYwMkie3mKB7H1KA_CfXflwDHw3yk-CeLrD_QpxUM1Ly_X9NiwuxFCXAuAzMfKA51ORrBNuLHtIMlMZyJeSKm4omGAWUcR-neLlUHYr9lp7fSxhxOxQMQVYheEieSl8OAghLG2_uCQo8ThVsd5twLT-htjZVMb_zu0e3F4zLYxzN9muiSD4BeIu_1t3QaqgtJXuplDBsH-dGLVtfo53QSS6v4zjfV3Nm2PBN44rfKjvtPCzYECmgpBKszrBpoeHA2j9j63M6Wh-d-GNbgw5jictUPNc6cubscmAHLnhv5EAoyav5TghXbeS-IFlPptOLdpkk1o2YsFhQFgV2toMzSRyyjmoH6MEtEjfTEUGheI-ffpyar9UNK4-HE9fE2J6qTs_5vV1DTCbb9XMbM18XeSuE1FuxcPkDyyMpAUu5tQVKRLj9jLo5Bv2EOQklgIvf7V71Apo\",\"stations\":{\"name\":\"d1\",\"location\":{\"type\":\"Point\",\"coordinates\":[48.4495,35.035]}},\"pollution\":[{\"probe_datetime\":\"2021-08-16T10:05:00Z\",\"no2\":41,\"so2\":11,\"o3\":3004,\"co\":821,\"pm25\":22,\"pm10\":31,\"environmant\":{\"temperature\":0,\"pressure\":0,\"humidity\":0}}]}";
	esp_err_t err = 0;
	int request_length = 0;
	//create a url for request
	char* request_data = malloc(1200 * sizeof(char));
	char string_value[15];
	char* access_token="{\"access-token\":\"eyJ0eXAiOiJKV1QiLCJhbGciOiJSUzI1NiJ9.eyJ1c2VyX2lkIjoxMywic29pbCI6Ijg5YjIzNGEyNzI2ZDQ4ODI4YmQxYzJkMzdiM2Q2ZDk4In0.o2c-cG_DmFXqISyU0hFi95Spmy-HU2sU3ROHNKyNwdek6ykDSHmQY-1Ucekj6jElVAyYXBBlx5tU_iegSQ6g_mMl3aQH6nTEnKwpknsZ_T11X7-NoOcYUovqKSY1wy2N5m85S4N8aMQMblPng5ULPGhbkrlcYwN5ntL6DhSAZ5bbSQBj_amcdzFAD-wRpDIEkyJDglzVkSZRW1k2qaANtDOAI-GQiSZsuLepiYwMkie3mKB7H1KA_CfXflwDHw3yk-CeLrD_QpxUM1Ly_X9NiwuxFCXAuAzMfKA51ORrBNuLHtIMlMZyJeSKm4omGAWUcR-neLlUHYr9lp7fSxhxOxQMQVYheEieSl8OAghLG2_uCQo8ThVsd5twLT-htjZVMb_zu0e3F4zLYxzN9muiSD4BeIu_1t3QaqgtJXuplDBsH-dGLVtfo53QSS6v4zjfV3Nm2PBN44rfKjvtPCzYECmgpBKszrBpoeHA2j9j63M6Wh-d-GNbgw5jictUPNc6cubscmAHLnhv5EAoyav5TghXbeS-IFlPptOLdpkk1o2YsFhQFgV2toMzSRyyjmoH6MEtEjfTEUGheI-ffpyar9UNK4-HE9fE2J6qTs_5vV1DTCbb9XMbM18XeSuE1FuxcPkDyyMpAUu5tQVKRLj9jLo5Bv2EOQklgIvf7V71Apo\", \"stations\": {\"name\":\"";
	strcpy(request_data,access_token);
	strcat(request_data,name);
	strcat(request_data, "\", \"location\": {\"type\": \"Point\", \"coordinates\": [");
	sprintf(string_value,  "%f", latitude);
	strcat(request_data,string_value);
	strcat(request_data,",");
	sprintf(string_value,  "%f",longitude);
	strcat(request_data,string_value);
	strcat(request_data, "]}}, \"pollution\": [{\"probe_datetime\": \"");
	strcat(request_data,probe_datatime);
	strcat(request_data, "\", \"co\":");
	sprintf(string_value,"%d", co);
	strcat(request_data,string_value);
	strcat(request_data, ", \"environmant\": {\"temperature\":");
	sprintf(string_value,"%d", temperature);
	strcat(request_data,string_value);
	strcat(request_data,", \"pressure\":");
	sprintf(string_value,"%d", presure);
	strcat(request_data,string_value);
	strcat(request_data, ", \"humidity\": ");
	sprintf(string_value,"%d", humidity);
	strcat(request_data,string_value);
	strcat(request_data, "}}]}");
	request_length = strlen(request_data);
	//config http client
	esp_http_client_config_t config ={
				.url = "https://dev.yourairtest.com/api/v1/stations/",
			    .event_handler = _http_event_handle,
				//.cert_pem = server_root_cert_pem_start,
			        	};
	 esp_http_client_handle_t client = esp_http_client_init(&config);
	    if(client == NULL){
	    	ESP_LOGE("http", "HTTP initilisation failed");
	    }
	    err = esp_http_client_set_method(client, HTTP_METHOD_POST);
	    if(err != ESP_OK){
	        	ESP_LOGE("HTTP", "%s", esp_err_to_name(err));
	        }
	    err = esp_http_client_set_header(client, "content-type", "application/json");
	    if(err != ESP_OK){
	    	ESP_LOGE("HTTP", "%s", esp_err_to_name(err));
	    }
	    err = esp_http_client_set_post_field(client, request_data, request_length);
	    if(err != ESP_OK){
	    	ESP_LOGE("HTTP", "%s", esp_err_to_name(err));
	    }
	    //send request
	    err = esp_http_client_perform(client);
	    if (err == ESP_OK) {
	    	ESP_LOGI("http", "HTTP POST status = %d, content-length = %d",
	    			esp_http_client_get_status_code(client),
					esp_http_client_get_content_length(client));
	    } else {
	    	ESP_LOGE("http", "HTTP POST request failed  - %s", esp_err_to_name(err));
	    }
	    esp_http_client_cleanup(client);

	   free(request_data);
}

