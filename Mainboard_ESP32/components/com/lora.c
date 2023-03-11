// Copyright 2023 PWr in Space, Krzysztof Gliwiński
#include "lora.h"

#define TAG "LORA"

lora_err_t lora_init(lora_struct_t *lora) {
  lora_err_t ret = LORA_OK;
  /*
   * Configure CPU hardware to communicate with the radio chip
   */
  lora->gpio_pad_select(lora->rst_gpio_num);
  lora->gpio_set_direction(lora->rst_gpio_num, GPIO_MODE_OUTPUT);
  lora->gpio_pad_select(lora->cs_gpio_num);
  lora->gpio_set_direction(lora->cs_gpio_num, GPIO_MODE_OUTPUT);

  /*
   * Perform hardware reset.
   */
  lora_reset(lora);

  /*
   * Check version.
   */
  uint8_t version;
  uint8_t i = 0;
  while (i++ < TIMEOUT_RESET) {
    version = lora_read_reg(lora, REG_VERSION);
    if (version == 0x12) break;
    lora->delay(2);
  }
  assert(i <= TIMEOUT_RESET + 1);  // at the end of the loop above, the max
                                   // value i can reach is TIMEOUT_RESET + 1

  ret |= lora_default_config(lora);

  return ret;
}

lora_err_t lora_default_config(lora_struct_t *lora) {
  lora_err_t ret = LORA_OK;
  lora_sleep(lora);
  ret |= lora_write_reg(lora, REG_FIFO_RX_BASE_ADDR, 0);
  ret |= lora_write_reg(lora, REG_FIFO_TX_BASE_ADDR, 0);
  ret |= lora_write_reg(lora, REG_LNA, lora_read_reg(lora, REG_LNA) | 0x03);
  ret |= lora_write_reg(lora, REG_MODEM_CONFIG_3, 0x04);
  lora_set_tx_power(lora, 17);

  lora_idle(lora);
  return ret;
}

lora_err_t lora_write_reg(lora_struct_t *lora, int16_t reg, int16_t val) {
  uint8_t out[2] = {0x80 | reg, val};
  uint8_t in[2];

  return lora->spi_transmit(in, out) == 1 ? LORA_OK : LORA_WRITE_ERR;
}

uint8_t lora_read_reg(lora_struct_t *lora, int16_t reg) {
  uint8_t out[2] = {reg, 0xff};
  uint8_t in[2];

  lora->spi_transmit(in, out);
  return in[1];
}

void lora_reset(lora_struct_t *lora) {
  lora->gpio_set_level(lora->rst_gpio_num, 0);
  lora->delay(pdMS_TO_TICKS(1));
  lora->gpio_set_level(lora->rst_gpio_num, 1);
  lora->delay(pdMS_TO_TICKS(10));
}

void lora_explicit_header_mode(lora_struct_t *lora) {
  lora->implicit = 0;
  lora_write_reg(lora, REG_MODEM_CONFIG_1,
                 lora_read_reg(lora, REG_MODEM_CONFIG_1) & 0xfe);
}

void lora_implicit_header_mode(lora_struct_t *lora, int16_t size) {
  lora->implicit = 1;
  lora_write_reg(lora, REG_MODEM_CONFIG_1,
                 lora_read_reg(lora, REG_MODEM_CONFIG_1) | 0x01);
  lora_write_reg(lora, REG_PAYLOAD_LENGTH, size);
}

void lora_idle(lora_struct_t *lora) {
  lora_write_reg(lora, REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
}

void lora_sleep(lora_struct_t *lora) {
  lora_write_reg(lora, REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_SLEEP);
}

void lora_set_receive_mode(lora_struct_t *lora) {
  lora_write_reg(lora, REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_RX_CONTINUOUS);
}

void lora_set_tx_power(lora_struct_t *lora, int16_t level) {
  // RF9x module uses PA_BOOST pin
  if (level < 2)
    level = 2;
  else if (level > 17)
    level = 17;
  lora_write_reg(lora, REG_PA_CONFIG, PA_BOOST | (level - 2));
}

void lora_set_frequency(lora_struct_t *lora, int32_t frequency) {
  lora->frequency = frequency;

  uint64_t frf = ((uint64_t)frequency << 19) / 32000000;

  lora_write_reg(lora, REG_FRF_MSB, (uint8_t)(frf >> 16));
  lora_write_reg(lora, REG_FRF_MID, (uint8_t)(frf >> 8));
  lora_write_reg(lora, REG_FRF_LSB, (uint8_t)(frf >> 0));
}

void lora_set_spreading_factor(lora_struct_t *lora, int16_t sf) {
  if (sf < 6)
    sf = 6;
  else if (sf > 12)
    sf = 12;

  if (sf == 6) {
    lora_write_reg(lora, REG_DETECTION_OPTIMIZE, 0xc5);
    lora_write_reg(lora, REG_DETECTION_THRESHOLD, 0x0c);
  } else {
    lora_write_reg(lora, REG_DETECTION_OPTIMIZE, 0xc3);
    lora_write_reg(lora, REG_DETECTION_THRESHOLD, 0x0a);
  }

  lora_write_reg(
      lora, REG_MODEM_CONFIG_2,
      (lora_read_reg(lora, REG_MODEM_CONFIG_2) & 0x0f) | ((sf << 4) & 0xf0));
}

void lora_set_bandwidth(lora_struct_t *lora, int32_t sbw) {
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

  lora_write_reg(lora, REG_MODEM_CONFIG_1,
                 (lora_read_reg(lora, REG_MODEM_CONFIG_1) & 0x0f) | (bw << 4));
}

void lora_set_coding_rate(lora_struct_t *lora, int16_t denominator) {
  if (denominator < 5)
    denominator = 5;
  else if (denominator > 8)
    denominator = 8;

  int16_t cr = denominator - 4;
  lora_write_reg(lora, REG_MODEM_CONFIG_1,
                 (lora_read_reg(lora, REG_MODEM_CONFIG_1) & 0xf1) | (cr << 1));
}

void lora_set_preamble_length(lora_struct_t *lora, int32_t length) {
  lora_write_reg(lora, REG_PREAMBLE_MSB, (uint8_t)(length >> 8));
  lora_write_reg(lora, REG_PREAMBLE_LSB, (uint8_t)(length >> 0));
}

void lora_set_sync_word(lora_struct_t *lora, int16_t sw) {
  lora_write_reg(lora, REG_SYNC_WORD, sw);
}

void lora_enable_crc(lora_struct_t *lora) {
  lora_write_reg(lora, REG_MODEM_CONFIG_2,
                 lora_read_reg(lora, REG_MODEM_CONFIG_2) | 0x04);
}

void lora_disable_crc(lora_struct_t *lora) {
  lora_write_reg(lora, REG_MODEM_CONFIG_2,
                 lora_read_reg(lora, REG_MODEM_CONFIG_2) & 0xfb);
}

// TODO(GLIBUS): Add a method with no delay, also add a breakout from while
// after some time
void lora_send_packet(lora_struct_t *lora, uint8_t *buf, int16_t size) {
  /*
   * Transfer data to radio.
   */
  lora_idle(lora);
  lora_write_reg(lora, REG_FIFO_ADDR_PTR, 0);

  for (int16_t i = 0; i < size; i++) {
    lora_write_reg(lora, REG_FIFO, *buf++);
  }
  lora_write_reg(lora, REG_PAYLOAD_LENGTH, size);

  /*
   * Start transmission and wait for conclusion.
   */
  lora_write_reg(lora, REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_TX);

  while ((lora_read_reg(lora, REG_IRQ_FLAGS) & IRQ_TX_DONE_MASK) == 0x00) {
    // int8_t read_reg = lora_read_reg(lora,REG_IRQ_FLAGS);
    // ESP_LOGI(TAG, "SEND FREEZES, reg: %04x", read_reg);
    lora->delay(2);
  }

  lora_write_reg(lora, REG_IRQ_FLAGS, IRQ_TX_DONE_MASK);
}

int16_t lora_receive_packet(lora_struct_t *lora, uint8_t *buf, int16_t size) {
  int16_t len = 0;

  /*
   * Check interrupts.
   */
  int16_t irq = lora_read_reg(lora, REG_IRQ_FLAGS);
  lora_write_reg(lora, REG_IRQ_FLAGS, irq);
  if ((irq & IRQ_RX_DONE_MASK) == 0) return 0;
  if (irq & IRQ_PAYLOAD_CRC_ERROR_MASK) return 0;

  /*
   * Find packet size.
   */
  if (lora->implicit) {
    len = lora_read_reg(lora, REG_PAYLOAD_LENGTH);
  } else {
    len = lora_read_reg(lora, REG_RX_NB_BYTES);
  }

  /*
   * Transfer data from radio.
   */
  lora_idle(lora);
  lora_write_reg(lora, REG_FIFO_ADDR_PTR,
                 lora_read_reg(lora, REG_FIFO_RX_CURRENT_ADDR));
  if (len > size) {
    len = size;
  }
  for (int16_t i = 0; i < len; i++) {
    buf[i] = lora_read_reg(lora, REG_FIFO);
  }

  return len;
}

int16_t lora_received(lora_struct_t *lora) {
  if (lora_read_reg(lora, REG_IRQ_FLAGS) & IRQ_RX_DONE_MASK) return 1;
  return 0;
}

int16_t lora_packet_rssi(lora_struct_t *lora) {
  return (lora_read_reg(lora, REG_PKT_RSSI_VALUE) -
          (lora->frequency < 868E6 ? 164 : 157));
}

// TODO(Glibus): check if this calculates properly
float lora_packet_snr(lora_struct_t *lora) {
  return ((int8_t)lora_read_reg(lora, REG_PKT_SNR_VALUE)) * 0.25;
}

void lora_close(lora_struct_t *lora) {
  lora_sleep(lora);
  // TODO(GLIBUS): sprawdzic co to robi i zaimplementowac
  //   close(__spi);  FIXME: end hardware features after lora_close
  //   close(__cs);
  //   close(__rst);
  //   __spi = -1;
  //   __cs = -1;
  //   __rst = -1;
}

void lora_dump_registers(lora_struct_t *lora) {
  int16_t i;
  printf("00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
  for (i = 0; i < 0x40; i++) {
    printf("%02X ", lora_read_reg(lora, i));
    if ((i & 0x0f) == 0x0f) printf("\n");
  }
  printf("\n");
}
