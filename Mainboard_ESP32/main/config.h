// Copyright 2023 PWr in Space, Krzysztof Gliwiński
#pragma once
#include "gpio.h"

/// \brief Pinout of ESP32 on ROSALIA
typedef enum {
  D0_LORA = GPIO_NUM_2,
  RS_LORA = GPIO_NUM_4,
  CS_LORA = GPIO_NUM_5,
  TDI = GPIO_NUM_12,
  TCK = GPIO_NUM_13,
  TMS = GPIO_NUM_14,
  TDO = GPIO_NUM_15,
  IGN = GPIO_NUM_16,
  ARM = GPIO_NUM_17,
  SPI_SCK = GPIO_NUM_18,
  SPI_MISO = GPIO_NUM_19,
  SPI_MOSI = GPIO_NUM_23,
  I2C_SDA = GPIO_NUM_21,
  I2C_SCL = GPIO_NUM_22,
  CS_FLASH = GPIO_NUM_26,
  CS_SD = GPIO_NUM_26,
  STATUS_LED = GPIO_NUM_27,
  PWM1 = GPIO_NUM_32,
  PWM2 = GPIO_NUM_33,
  LIM2_1 = GPIO_NUM_34,
  LIM2_2 = GPIO_NUM_35,
  LIM1_1 = GPIO_NUM_36,
  LIM1_2 = GPIO_NUM_39
} rosalia_pinout_gpio_t;

/// \brief Limit switches
