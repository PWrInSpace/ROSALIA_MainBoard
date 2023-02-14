// Copyright 2023 PWr in Space

#include "driver/uart.h"
#include "string.h"

// TODO(Glibus): add documentation when final version is written

void uart_interface_init(void);

void uart_transmit(char* msg);

bool uart_receive(char* buf);
