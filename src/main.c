/*  WiFi Tester Example

    A simple WiFi tester which scans the surrounding WiFi networks and allows 
    user to input the password and varifies the network.

    Author: WTT
*/

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"

static const char *TAG = "scan";

static esp_err_t event_handler(void *ctx, system_event_t *event) {
    switch (event->event_id) {
        case SYSTEM_EVENT_STA_START: {
            wifi_scan_config_t wifi_scanconfig = {
                .scan_type = WIFI_SCAN_TYPE_ACTIVE,
                .scan_time = {  .active = { .min = 0,
                                            .max = 0}},
                .show_hidden = true,
                .channel = 0
            };
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");
            ESP_ERROR_CHECK(esp_wifi_scan_start(&wifi_scanconfig, true));
            break;
        }

        case SYSTEM_EVENT_SCAN_DONE: {
            ESP_LOGI(TAG, "SYSTEM_EVENT_SCAN_DONE");
            uint16_t apCount = 0;
            esp_wifi_scan_get_ap_num(&apCount);
            printf("Total AP(s) found: %d\n", apCount);

            wifi_ap_record_t apList[10];
            uint16_t maxAp = 10;

            esp_wifi_scan_get_ap_records(&maxAp, apList);

            for (int i = 0; i < maxAp; i++) {
                wifi_ap_record_t *ap = apList + i;
                printf("#%d/%d | SSID: %s; RSSI: %d; Channel: %d \n", i, apCount, ap->ssid, ap->rssi, ap->primary);
            }
            fflush(stdout);
        }

        default:
            break;
    }
    return ESP_OK;
}

void app_main() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_start());
}