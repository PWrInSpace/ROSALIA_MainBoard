// Copyright 2022 PWrInSpace

#include "console_commands.h"

#include <stdlib.h>

#define TAG "CLI"

int CLI_echo(int argc, char **argv) {
  if (argc != 1) {
    return 1;
  }
  ESP_LOGI(TAG, argv[0]);
  return 0;
}
