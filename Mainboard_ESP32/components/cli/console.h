// Copyright 2022 PWrInSpace, Kuba

#ifndef CONSOLE_H
#define CONSOLE_H

#include "esp_console.h"

esp_err_t console_init(void);
esp_err_t console_register_commands(esp_console_cmd_t *commands,
                                    size_t number_of_cmd);
esp_err_t console_deinit(void);

#endif
