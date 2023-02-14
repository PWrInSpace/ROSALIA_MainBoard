// Copyright 2022 PWrInSpace

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "lora.h"
#include "uart_interface.h"

#define TAG "MAIN"

void app_main(void) {
  // esp_err_t init = lora_init();
  uart_interface_init();
  char buf[128];
  while (1) {
    // ESP_LOGI(TAG, "Test...");
    if (uart_receive(buf)) {
      ESP_LOGI(TAG, "Powinno wysłać...");
      uart_transmit(buf);
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
