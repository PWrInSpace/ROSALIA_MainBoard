// Copyright 2022 PWrInSpace

#include <stdlib.h>
#include "console_commands.h"
#include "state_machine.h"
#include "esp_log.h"
#include "esp_system.h"
#include "brake_servo.h"
#include "recovery_servo.h"
#include "flash_nvs.h"
#include "rtos_tasks.h"
#include "nvs.h"

#define TAG "CLI"

int CLI_echo(int argc, char **argv) {
    uint8_t state = SM_get_current_state();
    ESP_LOGI(TAG, "Current state %d", state);
    return 0;
}
