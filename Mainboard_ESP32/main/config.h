// Copyright 2023 PWr in Space, Krzysztof Gliwiński
#pragma once
#include "driver/gpio.h"

/// \brief Pinout of ESP32 on ROSALIA

#define D0_LORA GPIO_NUM_2
#define RS_LORA GPIO_NUM_4
#define CS_LORA GPIO_NUM_5
#define TDI GPIO_NUM_12
#define TCK GPIO_NUM_13
#define TMS GPIO_NUM_14
#define TDO GPIO_NUM_15
#define IGN GPIO_NUM_16
#define ARM GPIO_NUM_17
#define SPI_SCK GPIO_NUM_18
#define SPI_MISO GPIO_NUM_19
#define SPI_MOSI GPIO_NUM_23
#define I2C_SDA GPIO_NUM_21
#define I2C_SCL GPIO_NUM_22
#define CS_FLASH GPIO_NUM_26
#define CS_SD GPIO_NUM_26
#define STATUS_LED GPIO_NUM_27
#define PWM1 GPIO_NUM_32
#define PWM2 GPIO_NUM_33
#define LIM2_1 GPIO_NUM_34
#define LIM2_2 GPIO_NUM_35
#define LIM1_1 GPIO_NUM_36
#define LIM1_2 GPIO_NUM_39

