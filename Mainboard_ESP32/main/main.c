// Copyright 2022 PWrInSpace

#include "console_commands.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#define TAG "MAIN"

esp_console_cmd_t console_commands[] = {
  ""
}

void app_main(void) {
  while (1) {
    ESP_LOGI(TAG, "Test...");
    // vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
