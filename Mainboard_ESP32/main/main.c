// Copyright 2022 PWrInSpace

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "lora.h"

#define TAG "MAIN"

void uart_init(void) {
  const uart_config_t uart_config = {
      .baud_rate = 115200,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .source_clk = UART_SCLK_APB,
  };

  // We won't use a buffer for sending data.
  uart_driver_install(UART, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
  uart_param_config(UART, &uart_config);
  uart_set_pin(UART, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

void app_main(void) {
  esp_err_t init = lora_init();

  while (1) {
    ESP_LOGI(TAG, "Test...");
    // vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
