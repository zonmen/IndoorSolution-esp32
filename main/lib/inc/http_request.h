#ifndef HTTP_REQUEST_HEADERFILE_H
#define HTTP_REQUEST_HEADERFILE_H


#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_client.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

extern int flag_status_http_sent;

esp_err_t _http_event_handle(esp_http_client_event_t *evt);

void https_post_request(char* name, float latitude, float longitude, char* probe_datatime, int co, int temperature, int presure, int humidity);

#endif
