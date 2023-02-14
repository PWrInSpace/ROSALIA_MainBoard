// Copyright 2022 PWrInSpace

#include <stdio.h>
#include <string.h>

#include "driver/uart.h"
#include "esp_console.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_vfs_dev.h"
#include "esp_vfs_fat.h"
#include "freertos/FreeRTOS.h"
#include "lora.h"

#define TAG "MAIN"

void app_main(void) {
  int16_t init = lora_init();

  vTaskDelay(100 / portTICK_PERIOD_MS);

  if (init != 1) {
    ESP_LOGI(TAG, "Lora init unsuccessful");
  } else {
    ESP_LOGI(TAG, "G");
  }
  ESP_ERROR_CHECK(
      uart_driver_install(CONFIG_ESP_CONSOLE_UART_NUM, 256, 0, 0, NULL, 0));
  esp_vfs_dev_uart_use_driver(CONFIG_ESP_CONSOLE_UART_NUM);
  esp_vfs_dev_uart_port_set_rx_line_endings(CONFIG_ESP_CONSOLE_UART_NUM,
                                            ESP_LINE_ENDINGS_CR);
  esp_vfs_dev_uart_port_set_tx_line_endings(CONFIG_ESP_CONSOLE_UART_NUM,
                                            ESP_LINE_ENDINGS_CRLF);
  char chr[10];
  while (1) {
    printf("Enter Data : ");
    scanf("%9s", chr);
    printf("\nData entered: %s\n", chr);
  }
}
