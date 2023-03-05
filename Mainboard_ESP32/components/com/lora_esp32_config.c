// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#include "lora_esp32_config.h"

static spi_device_handle_t __spi;

#define CONFIG_CS_GPIO GPIO_NUM_5
#define CONFIG_SCK_GPIO GPIO_NUM_18
#define CONFIG_MOSI_GPIO GPIO_NUM_23
#define CONFIG_MISO_GPIO GPIO_NUM_19
#define CONFIG_RST_GPIO GPIO_NUM_4

bool lora_spi_init() {
  esp_err_t ret;
  spi_bus_config_t bus = {.miso_io_num = CONFIG_MISO_GPIO,
                          .mosi_io_num = CONFIG_MOSI_GPIO,
                          .sclk_io_num = CONFIG_SCK_GPIO,
                          .quadwp_io_num = -1,
                          .quadhd_io_num = -1,
                          .max_transfer_sz = 0};

  ret = spi_bus_initialize(VSPI_HOST, &bus, 0);
  assert(ret == ESP_OK);

  spi_device_interface_config_t dev = {.clock_speed_hz = 9000000,
                                       .mode = 0,
                                       .spics_io_num = -1,
                                       .queue_size = 1,
                                       .flags = 0,
                                       .pre_cb = NULL};
  ret = spi_bus_add_device(VSPI_HOST, &dev, &__spi);
  assert(ret == ESP_OK);
  return ret == ESP_OK ? true : false;
}

bool spi_transmit(uint8_t _in[2], uint8_t _out[2]) {
  spi_transaction_t t = {.flags = 0,
                         .length = 8 * sizeof(_out),
                         .tx_buffer = _out,
                         .rx_buffer = _in};

  gpio_set_level(CONFIG_CS_GPIO, 0);
  spi_device_transmit(__spi, &t);
  gpio_set_level(CONFIG_CS_GPIO, 1);
  return true;
}