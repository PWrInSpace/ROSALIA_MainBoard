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

void task_tx(void *p) {
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(5000));
    lora_send_packet((uint8_t *)"Hello", 5);
    printf("packet sent...\n");
  }
}

void app_main(void) {
  int16_t init = lora_init();
  // lora_reset();

  vTaskDelay(pdMS_TO_TICKS(100));

  if (init != 1) {
    ESP_LOGI(TAG, "Lora init unsuccessful");
  } else {
    ESP_LOGI(TAG, "G");
  }

  lora_set_frequency(867e6);
  lora_set_bandwidth(250e3);
  lora_disable_crc();

  int16_t read_val_one = lora_read_reg(0x0d);
  int16_t read_val_two = lora_read_reg(0x0c);
  ESP_LOGI(TAG, "LORA_READ: %04x, %04x", read_val_one, read_val_two);

  ESP_ERROR_CHECK(
      uart_driver_install(CONFIG_ESP_CONSOLE_UART_NUM, 256, 0, 0, NULL, 0));
  esp_vfs_dev_uart_use_driver(CONFIG_ESP_CONSOLE_UART_NUM);
  esp_vfs_dev_uart_port_set_rx_line_endings(CONFIG_ESP_CONSOLE_UART_NUM,
                                            ESP_LINE_ENDINGS_CR);
  esp_vfs_dev_uart_port_set_tx_line_endings(CONFIG_ESP_CONSOLE_UART_NUM,
                                            ESP_LINE_ENDINGS_CRLF);
  char chars[10] = "HELLO!";
  xTaskCreate(&task_tx, "task_tx", 2048, NULL, 5, NULL);
}
