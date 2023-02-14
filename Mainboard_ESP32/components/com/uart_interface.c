// Copyright 2023 PWr in Space

#include "uart_interface.h"

// TODO(Glibus): change this to some kind of struct
static uart_port_t uart_num = UART_NUM_0;
static QueueHandle_t uart_queue;

void uart_interface_init(void) {
  uart_config_t uart_config = {.baud_rate = 115200,
                               .data_bits = UART_DATA_8_BITS,
                               .parity = UART_PARITY_DISABLE,
                               .stop_bits = UART_STOP_BITS_1,
                               .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
                               .rx_flow_ctrl_thresh = 122,
                               .source_clk = UART_SCLK_APB};

  // set uart pins
  ESP_ERROR_CHECK(uart_set_pin(uart_num, 1, 3, 22, 19));

  // Configure UART parameters
  ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));

  const int uart_buffer_size = (1024 * 2);

  // Install UART driver using an event queue here
  ESP_ERROR_CHECK(uart_driver_install(uart_num, uart_buffer_size,
                                      uart_buffer_size, 10, &uart_queue, 0));
}

void uart_transmit(char* msg) {
  msg = "dupa";
  uart_write_bytes(uart_num, msg, strlen(msg));
}

bool uart_receive(char* buf) {
  //   uint8_t data[128];
  size_t length = 0;
  ESP_ERROR_CHECK(uart_get_buffered_data_len(uart_num, &length));
  length = uart_read_bytes(uart_num, buf, length, 100);
  return length > 0;
}
