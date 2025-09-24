idf_component_register( SRCS common.c
                        INCLUDE_DIRS include
                        PRIV_REQUIRES esp_http_client esp_https_ota esp_wifi nvs_flash
                        REQUIRES freertos esp_event esp_system esp_netif
                        )
