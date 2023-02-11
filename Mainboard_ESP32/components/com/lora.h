// Copyright 2023 PWr in Space, Krzysztof Gliwiński
#pragma once

#include <stdint.h>
#include <string.h>

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
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
 * \brief Write a value to a register.
 * \param reg Register index.
 * \param val Value to write.
 */
void lora_write_reg(int16_t reg, int16_t val);

/*!
 * \brief Read the current value of a register.
 * \param reg Register index.
 * \return Value of the register.
 */
int16_t lora_read_reg(int16_t reg);

/*!
 * \brief Perform physical reset on the Lora chip
 */
void lora_reset(void);

/*!
 * \brief Configure explicit header mode.
 * Packet size will be included in the frame.
 */
void lora_explicit_header_mode(void);

/*!
 * \brief Configure implicit header mode.
 * All packets will have a predefined size.
 * \param size Size of the packets.
 */
void lora_implicit_header_mode(int16_t size);

/*!
 * \brief Sets the radio transceiver in idle mode.
 * \note Must be used to change registers and access the FIFO.
 */
void lora_idle(void);

/*!
 * \brief Sets the radio transceiver in sleep mode.
 * \note Low power consumption and FIFO is lost.
 */
void lora_sleep(void);

/*!
 * \brief Sets the radio transceiver in receive mode.
 * \note Incoming packets will be received.
 */
void lora_receive(void);

/*!
 * \brief Configure power level for transmission
 * \param level 2-17, from least to most power
 */
void lora_set_tx_power(int16_t level);

/*!
 * \brief Set carrier frequency.
 * \param frequency Frequency in Hz
 */
void lora_set_frequency(int32_t frequency);

/*!
 * \brief Set spreading factor.
 * \param sf 6-12, Spreading factor to use.
 */
void lora_set_spreading_factor(int16_t sf);

/*!
 * \brief Set bandwidth (bit rate)
 * \param sbw Bandwidth in Hz (up to 500000)
 */
void lora_set_bandwidth(int32_t sbw);

/*!
 * \brief Set coding rate
 * \param denominator 5-8, Denominator for the coding rate 4/x
 */
void lora_set_coding_rate(int16_t denominator);

/*!
 * \brief Set the size of preamble.
 * \param length Preamble length in symbols.
 */
void lora_set_preamble_length(int32_t length);

/*!
 * \brief Change radio sync word.
 * \param sw New sync word to use.
 */
void lora_set_sync_word(int16_t sw);

/*!
 * \brief Enable appending/verifying packet CRC.
 */
void lora_enable_crc(void);

/*!
 * \brief Disable appending/verifying packet CRC.
 */
void lora_disable_crc(void);

/*!
 * \brief Perform hardware initialization.
 */
int16_t lora_init(void);

/*!
 * \brief Send a packet.
 * \param buf Data to be sent
 * \param size Size of data.
 */
void lora_send_packet(uint8_t *buf, int16_t size);

/*!
 * \brief Read a received packet.
 * \param buf Buffer for the data.
 * \param size Available size in buffer (bytes).
 * \return Number of bytes received (zero if no packet available).
 */
int16_t lora_receive_packet(uint8_t *buf, int16_t size);

/*!
 * \returns non-zero if there is data to read (packet received).
 */
int16_t lora_received(void);

/*!
 * \returns last packet's RSSI.
 */
int16_t lora_packet_rssi(void);

/*!
 * \returns last packet's SNR (signal to noise ratio).
 */
float lora_packet_snr(void);

/*!
 * \brief Shutdown hardware.
 */
void lora_close(void);

/// Not supported
int16_t lora_initialized(void);

void lora_dump_registers(void);
