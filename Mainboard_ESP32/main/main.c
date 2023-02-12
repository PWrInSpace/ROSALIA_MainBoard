// Copyright 2022 PWrInSpace

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "lora.h"

#define TAG "MAIN"


void app_main(void) {
  esp_err_t init = lora_init();

  while (1) {
    ESP_LOGI(TAG, "Test...");
    // vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
