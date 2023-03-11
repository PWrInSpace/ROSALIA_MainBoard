// Copyright 2023 PWr in Space, Krzysztof Gliwiński
#pragma once

#include <stdint.h>
#include <string.h>

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rom/gpio.h"
#include "soc/gpio_struct.h"

/*
 * IRQ masks
 */
#define IRQ_TX_DONE_MASK 0x08
#define IRQ_PAYLOAD_CRC_ERROR_MASK 0x20
#define IRQ_RX_DONE_MASK 0x40

#define PA_OUTPUT_RFO_PIN 0
#define PA_OUTPUT_PA_BOOST_PIN 1

#define TIMEOUT_RESET 100

/*
 * Register definitions
 */
#define REG_FIFO 0x00
#define REG_OP_MODE 0x01
#define REG_FRF_MSB 0x06
#define REG_FRF_MID 0x07
#define REG_FRF_LSB 0x08
#define REG_PA_CONFIG 0x09
#define REG_LNA 0x0c
#define REG_FIFO_ADDR_PTR 0x0d
#define REG_FIFO_TX_BASE_ADDR 0x0e
#define REG_FIFO_RX_BASE_ADDR 0x0f
#define REG_FIFO_RX_CURRENT_ADDR 0x10
#define REG_IRQ_FLAGS 0x12
#define REG_RX_NB_BYTES 0x13
#define REG_PKT_SNR_VALUE 0x19
#define REG_PKT_RSSI_VALUE 0x1a
#define REG_MODEM_CONFIG_1 0x1d
#define REG_MODEM_CONFIG_2 0x1e
#define REG_PREAMBLE_MSB 0x20
#define REG_PREAMBLE_LSB 0x21
#define REG_PAYLOAD_LENGTH 0x22
#define REG_MODEM_CONFIG_3 0x26
#define REG_RSSI_WIDEBAND 0x2c
#define REG_DETECTION_OPTIMIZE 0x31
#define REG_DETECTION_THRESHOLD 0x37
#define REG_SYNC_WORD 0x39
#define REG_DIO_MAPPING_1 0x40
#define REG_VERSION 0x42

/*
 * Transceiver modes
 */
#define MODE_LONG_RANGE_MODE 0x80
#define MODE_SLEEP 0x00
#define MODE_STDBY 0x01
#define MODE_TX 0x03
#define MODE_RX_CONTINUOUS 0x05
#define MODE_RX_SINGLE 0x06

/*
 * PA configuration
 */
#define PA_BOOST 0x80

/*!
  \brief Lora functions return values enum
*/
typedef enum {
  LORA_OK = 0,
  LORA_INIT_ERR,
  LORA_WRITE_ERR,
  LORA_TRANSMIT_ERR,
  LORA_RECEIVE_ERR
} lora_err_t;

typedef enum {
  LORA_GPIO_MODE_DISABLE = 0,
  LORA_GPIO_MODE_INPUT,
  LORA_GPIO_MODE_OUTPUT
} lora_gpio_mode_t;

typedef bool (*lora_SPI_transmit)(uint8_t _in[2], uint8_t _val[2]);
typedef void (*lora_delay)(size_t _ms);
typedef bool (*lora_GPIO_set_level)(uint8_t _gpio_num, uint32_t _level);
typedef void (*lora_GPIO_pad_select_gpio)(uint8_t _gpio_num);
typedef bool (*lora_GPIO_set_direction)(uint8_t _gpio_num,
                                        lora_gpio_mode_t _direction);
typedef void (*lora_log)(const char *info);

typedef struct {
  lora_SPI_transmit spi_transmit;
  lora_delay delay;
  lora_GPIO_set_level gpio_set_level;
  lora_GPIO_pad_select_gpio gpio_pad_select;
  lora_GPIO_set_direction gpio_set_direction;
  lora_log log;
  uint8_t rst_gpio_num;
  uint8_t cs_gpio_num;
  uint8_t d0_gpio_num;
  int16_t implicit;
  int32_t frequency;
} lora_struct_t;

/*!
 * \brief Perform hardware initialization.
 */
lora_err_t lora_init(lora_struct_t *lora);

/*!
 * \brief Create default config and set parameters
 */
lora_err_t lora_default_config(lora_struct_t *lora);

/*!
 * \brief Write a value to a register.
 * \param reg Register index.
 * \param val Value to write.
 * \return lora_err_t value
 */
lora_err_t lora_write_reg(lora_struct_t *lora, int16_t reg, int16_t val);

/*!
 * \brief Read the current value of a register.
 * \param reg Register index.
 * \returns Value of the register.
 */
uint8_t lora_read_reg(lora_struct_t *lora, int16_t reg);

/*!
 * \brief Perform physical reset on the Lora chip
 */
void lora_reset(lora_struct_t *lora);

/*!
 * \brief Configure explicit header mode.
 * Packet size will be included in the frame.
 */
void lora_explicit_header_mode(lora_struct_t *lora);

/*!
 * \brief Configure implicit header mode.
 * All packets will have a predefined size.
 * \param size Size of the packets.
 */
void lora_implicit_header_mode(lora_struct_t *lora, int16_t size);

/*!
 * \brief Sets the radio transceiver in idle mode.
 * \note Must be used to change registers and access the FIFO.
 */
void lora_idle(lora_struct_t *lora);

/*!
 * \brief Sets the radio transceiver in sleep mode.
 * \note Low power consumption and FIFO is lost.
 */
void lora_sleep(lora_struct_t *lora);

/*!
 * \brief Sets the radio transceiver in receive mode.
 * \note Incoming packets will be received.
 */
void lora_set_receive_mode(lora_struct_t *lora);

/*!
 * \brief Configure power level for transmission
 * \param level 2-17, from least to most power
 */
void lora_set_tx_power(lora_struct_t *lora, int16_t level);

/*!
 * \brief Set carrier frequency.
 * \param frequency Frequency in Hz
 */
void lora_set_frequency(lora_struct_t *lora, int32_t frequency);

/*!
 * \brief Set spreading factor.
 * \param sf 6-12, Spreading factor to use.
 */
void lora_set_spreading_factor(lora_struct_t *lora, int16_t sf);

/*!
 * \brief Set bandwidth (bit rate)
 * \param sbw Bandwidth in Hz (up to 500000)
 */
void lora_set_bandwidth(lora_struct_t *lora, int32_t sbw);

/*!
 * \brief Set coding rate
 * \param denominator 5-8, Denominator for the coding rate 4/x
 */
void lora_set_coding_rate(lora_struct_t *lora, int16_t denominator);

/*!
 * \brief Set the size of preamble.
 * \param length Preamble length in symbols.
 */
void lora_set_preamble_length(lora_struct_t *lora, int32_t length);

/*!
 * \brief Change radio sync word.
 * \param sw New sync word to use.
 */
void lora_set_sync_word(lora_struct_t *lora, int16_t sw);

/*!
 * \brief Enable appending/verifying packet CRC.
 */
void lora_enable_crc(lora_struct_t *lora);

/*!
 * \brief Disable appending/verifying packet CRC.
 */
void lora_disable_crc(lora_struct_t *lora);

/*!
 * \brief Send a packet.
 * \param buf Data to be sent
 * \param size Size of data.
 */
void lora_send_packet(lora_struct_t *lora, uint8_t *buf, int16_t size);

/*!
 * \brief Read a received packet.
 * \param buf Buffer for the data.
 * \param size Available size in buffer (bytes).
 * \returns Number of bytes received (zero if no packet available).
 */
int16_t lora_receive_packet(lora_struct_t *lora, uint8_t *buf, int16_t size);

/*!
 * \returns non-zero if there is data to read (packet received).
 */
int16_t lora_received(lora_struct_t *lora);

/*!
 * \returns last packet's RSSI.
 */
int16_t lora_packet_rssi(lora_struct_t *lora);

/*!
 * \returns last packet's SNR (signal to noise ratio).
 */
float lora_packet_snr(lora_struct_t *lora);

/*!
 * \brief Shutdown hardware.
 */
void lora_close(lora_struct_t *lora);

/// \brief Not supported
int16_t lora_initialized(lora_struct_t *lora);

/// \brief Dump registers :D
// TODO(Glibus): rather than printf the output save it to some kind of struct
void lora_dump_registers(lora_struct_t *lora);
