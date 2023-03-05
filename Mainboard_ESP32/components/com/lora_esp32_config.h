// Copyright 2023 PWr in Space, Gliwuś
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "config.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"

bool lora_spi_init();

bool spi_transmit(uint8_t _in[2], uint8_t _out[2]);