// Copyright 2023 PWr in Space, Krzysztof Gliwiński
#include "lora.h"

static spi_device_handle_t __spi;

static int16_t __implicit;
static int32_t __frequency;

// TODO(Glibus): change this to be initiated in function (ptr to function)
#define CONFIG_CS_GPIO GPIO_NUM_5
#define CONFIG_SCK_GPIO GPIO_NUM_18
#define CONFIG_MOSI_GPIO GPIO_NUM_23
#define CONFIG_MISO_GPIO GPIO_NUM_19
#define CONFIG_RST_GPIO GPIO_NUM_4
#define TAG "LORA"
void lora_write_reg(lora_struct_t *lora, int16_t reg, int16_t val) {
  uint8_t out[2] = {0x80 | reg, val};
  uint8_t in[2];

  lora->spi_transmit(in, out);
  // spi_transaction_t t = {
  //     .flags = 0, .length = 8 * sizeof(out), .tx_buffer = out, .rx_buffer = in};

  // gpio_set_level(CONFIG_CS_GPIO, 0);
  // spi_device_transmit(__spi, &t);
  // gpio_set_level(CONFIG_CS_GPIO, 1);
  
}

int16_t lora_read_reg(int16_t reg) {
  uint8_t out[2] = {reg, 0xff};
  uint8_t in[2];

  spi_transaction_t t = {
      .flags = 0, .length = 8 * sizeof(out), .tx_buffer = out, .rx_buffer = in};

  gpio_set_level(CONFIG_CS_GPIO, 0);
  spi_device_transmit(__spi, &t);
  gpio_set_level(CONFIG_CS_GPIO, 1);
  return in[1];
}

void lora_reset(void) {
  gpio_set_level(CONFIG_RST_GPIO, 0);
  vTaskDelay(pdMS_TO_TICKS(1));
  gpio_set_level(CONFIG_RST_GPIO, 1);
  vTaskDelay(pdMS_TO_TICKS(10));
}

void lora_explicit_header_mode(void) {
  __implicit = 0;
  lora_write_reg(REG_MODEM_CONFIG_1, lora_read_reg(REG_MODEM_CONFIG_1) & 0xfe);
}

void lora_implicit_header_mode(int16_t size) {
  __implicit = 1;
  lora_write_reg(REG_MODEM_CONFIG_1, lora_read_reg(REG_MODEM_CONFIG_1) | 0x01);
  lora_write_reg(REG_PAYLOAD_LENGTH, size);
}

void lora_idle(void) {
  lora_write_reg(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
}

void lora_sleep(void) {
  lora_write_reg(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_SLEEP);
}

void lora_receive(void) {
  lora_write_reg(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_RX_CONTINUOUS);
}

void lora_set_tx_power(int16_t level) {
  // RF9x module uses PA_BOOST pin
  if (level < 2)
    level = 2;
  else if (level > 17)
    level = 17;
  lora_write_reg(REG_PA_CONFIG, PA_BOOST | (level - 2));
}

void lora_set_frequency(int32_t frequency) {
  __frequency = frequency;

  uint64_t frf = ((uint64_t)frequency << 19) / 32000000;

  lora_write_reg(REG_FRF_MSB, (uint8_t)(frf >> 16));
  lora_write_reg(REG_FRF_MID, (uint8_t)(frf >> 8));
  lora_write_reg(REG_FRF_LSB, (uint8_t)(frf >> 0));
}

void lora_set_spreading_factor(int16_t sf) {
  if (sf < 6)
    sf = 6;
  else if (sf > 12)
    sf = 12;

  if (sf == 6) {
    lora_write_reg(REG_DETECTION_OPTIMIZE, 0xc5);
    lora_write_reg(REG_DETECTION_THRESHOLD, 0x0c);
  } else {
    lora_write_reg(REG_DETECTION_OPTIMIZE, 0xc3);
    lora_write_reg(REG_DETECTION_THRESHOLD, 0x0a);
  }

  lora_write_reg(
      REG_MODEM_CONFIG_2,
      (lora_read_reg(REG_MODEM_CONFIG_2) & 0x0f) | ((sf << 4) & 0xf0));
}

void lora_set_bandwidth(int32_t sbw) {
  int16_t bw;

  if (sbw <= 7.8E3)
    bw = 0;
  else if (sbw <= 10.4E3)
    bw = 1;
  else if (sbw <= 15.6E3)
    bw = 2;
  else if (sbw <= 20.8E3)
    bw = 3;
  else if (sbw <= 31.25E3)
    bw = 4;
  else if (sbw <= 41.7E3)
    bw = 5;
  else if (sbw <= 62.5E3)
    bw = 6;
  else if (sbw <= 125E3)
    bw = 7;
  else if (sbw <= 250E3)
    bw = 8;
  else
    bw = 9;
  lora_write_reg(REG_MODEM_CONFIG_1,
                 (lora_read_reg(REG_MODEM_CONFIG_1) & 0x0f) | (bw << 4));
}

void lora_set_coding_rate(int16_t denominator) {
  if (denominator < 5)
    denominator = 5;
  else if (denominator > 8)
    denominator = 8;

  int16_t cr = denominator - 4;
  lora_write_reg(REG_MODEM_CONFIG_1,
                 (lora_read_reg(REG_MODEM_CONFIG_1) & 0xf1) | (cr << 1));
}

void lora_set_preamble_length(int32_t length) {
  lora_write_reg(REG_PREAMBLE_MSB, (uint8_t)(length >> 8));
  lora_write_reg(REG_PREAMBLE_LSB, (uint8_t)(length >> 0));
}

void lora_set_sync_word(int16_t sw) { lora_write_reg(REG_SYNC_WORD, sw); }

void lora_enable_crc(void) {
  lora_write_reg(REG_MODEM_CONFIG_2, lora_read_reg(REG_MODEM_CONFIG_2) | 0x04);
}

void lora_disable_crc(void) {
  lora_write_reg(REG_MODEM_CONFIG_2, lora_read_reg(REG_MODEM_CONFIG_2) & 0xfb);
}

int16_t lora_init(void) {
  esp_err_t ret;

  /*
   * Configure CPU hardware to communicate with the radio chip
   */
  gpio_pad_select_gpio(CONFIG_RST_GPIO);
  gpio_set_direction(CONFIG_RST_GPIO, GPIO_MODE_OUTPUT);
  gpio_pad_select_gpio(CONFIG_CS_GPIO);
  gpio_set_direction(CONFIG_CS_GPIO, GPIO_MODE_OUTPUT);

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

  /*
   * Perform hardware reset.
   */
  lora_reset();

  /*
   * Check version.
   */
  uint8_t version;
  uint8_t i = 0;
  while (i++ < TIMEOUT_RESET) {
    version = lora_read_reg(REG_VERSION);
    if (version == 0x12) break;
    vTaskDelay(2);
  }
  assert(i <= TIMEOUT_RESET + 1);  // at the end of the loop above, the max
                                   // value i can reach is TIMEOUT_RESET + 1

  /*
   * Default configuration.
   */
  lora_sleep();
  lora_write_reg(REG_FIFO_RX_BASE_ADDR, 0);
  lora_write_reg(REG_FIFO_TX_BASE_ADDR, 0);
  lora_write_reg(REG_LNA, lora_read_reg(REG_LNA) | 0x03);
  lora_write_reg(REG_MODEM_CONFIG_3, 0x04);
  lora_set_tx_power(17);

  lora_idle();
  return 1;
}

// TODO(GLIBUS): Add a method with no delay
void lora_send_packet(uint8_t *buf, int16_t size) {
  /*
   * Transfer data to radio.
   */
  lora_idle();
  lora_write_reg(REG_FIFO_ADDR_PTR, 0);

  for (int16_t i = 0; i < size; i++) {
    lora_write_reg(REG_FIFO, *buf++);
  }
  lora_write_reg(REG_PAYLOAD_LENGTH, size);

  /*
   * Start transmission and wait for conclusion.
   */
  lora_write_reg(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_TX);

  while ((lora_read_reg(REG_IRQ_FLAGS) & IRQ_TX_DONE_MASK) == 0x00) {
    // int8_t read_reg = lora_read_reg(REG_IRQ_FLAGS);
    // ESP_LOGI(TAG, "SEND FREEZES, reg: %04x", read_reg);
    vTaskDelay(2);
  }

  lora_write_reg(REG_IRQ_FLAGS, IRQ_TX_DONE_MASK);
}

int16_t lora_receive_packet(uint8_t *buf, int16_t size) {
  int16_t len = 0;

  /*
   * Check interrupts.
   */
  int16_t irq = lora_read_reg(REG_IRQ_FLAGS);
  lora_write_reg(REG_IRQ_FLAGS, irq);
  if ((irq & IRQ_RX_DONE_MASK) == 0) return 0;
  if (irq & IRQ_PAYLOAD_CRC_ERROR_MASK) return 0;

  /*
   * Find packet size.
   */
  if (__implicit) {
    len = lora_read_reg(REG_PAYLOAD_LENGTH);
  } else {
    len = lora_read_reg(REG_RX_NB_BYTES);
  }

  /*
   * Transfer data from radio.
   */
  lora_idle();
  lora_write_reg(REG_FIFO_ADDR_PTR, lora_read_reg(REG_FIFO_RX_CURRENT_ADDR));
  if (len > size) {
    len = size;
  }
  for (int16_t i = 0; i < len; i++) {
    *buf++ = lora_read_reg(REG_FIFO);
  }

  return len;
}

int16_t lora_received(void) {
  if (lora_read_reg(REG_IRQ_FLAGS) & IRQ_RX_DONE_MASK) return 1;
  return 0;
}

int16_t lora_packet_rssi(void) {
  return (lora_read_reg(REG_PKT_RSSI_VALUE) -
          (__frequency < 868E6 ? 164 : 157));
}

float lora_packet_snr(void) {
  return ((int8_t)lora_read_reg(REG_PKT_SNR_VALUE)) * 0.25;
}

void lora_close(void) {
  lora_sleep();
  // TODO(GLIBUS): sprawdzic co to robi i zaimplementowac
  //   close(__spi);  FIXME: end hardware features after lora_close
  //   close(__cs);
  //   close(__rst);
  //   __spi = -1;
  //   __cs = -1;
  //   __rst = -1;
}

void lora_dump_registers(void) {
  int16_t i;
  printf("00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
  for (i = 0; i < 0x40; i++) {
    printf("%02X ", lora_read_reg(i));
    if ((i & 0x0f) == 0x0f) printf("\n");
  }
  printf("\n");
}
