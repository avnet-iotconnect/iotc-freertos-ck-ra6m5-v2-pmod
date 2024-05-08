/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : icp_20100.h
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

/*
 * icp_20100.h
 *
 *  Created on: 10-Mar-2023
 *      Author: 31342
 */

#ifndef icp_20100_H_
#define icp_20100_H_

#define ICP_I2C_ERROR      (-1)
#include "r_typedefs.h"
/**
 * @brief Pressure 20 register map.
 * @details Specified register map of Pressure 20 Click driver.
 */
#define PRESSURE20_REG_TRIM1_MSB                (0x05)
#define PRESSURE20_REG_TRIM2_LSB                (0x06)
#define PRESSURE20_REG_TRIM2_MSB                (0x07)
#define PRESSURE20_REG_DEVICE_ID                (0x0C)
#define PRESSURE20_REG_IO_DRIVE_STRENGTH        (0x0D)
#define PRESSURE20_REG_OTP_CONFIG1              (0xAC)
#define PRESSURE20_REG_OTP_MR_LSB               (0xAD)
#define PRESSURE20_REG_OTP_MR_MSB               (0xAE)
#define PRESSURE20_REG_OTP_MRA_LSB              (0xAF)
#define PRESSURE20_REG_OTP_MRA_MSB              (0xB0)
#define PRESSURE20_REG_OTP_MRB_LSB              (0xB1)
#define PRESSURE20_REG_OTP_MRB_MSB              (0xB2)
#define PRESSURE20_REG_OTP_ADDRESS_REG          (0xB5)
#define PRESSURE20_REG_OTP_COMMAND_REG          (0xB6)
#define PRESSURE20_REG_OTP_RDATA                (0xB8)
#define PRESSURE20_REG_OTP_STATUS               (0xB9)
#define PRESSURE20_REG_OTP_DBG2                 (0xBC)
#define PRESSURE20_REG_MASTER_LOCK              (0xBE)
#define PRESSURE20_REG_OTP_STATUS2              (0xBF)
#define PRESSURE20_REG_MODE_SELECT              (0xC0)
#define PRESSURE20_REG_INTERRUPT_STATUS         (0xC1)
#define PRESSURE20_REG_INTERRUPT_MASK           (0xC2)
#define PRESSURE20_REG_FIFO_CONFIG              (0xC3)
#define PRESSURE20_REG_FIFO_FILL                (0xC4)
#define PRESSURE20_REG_SPI_MODE                 (0xC5)
#define PRESSURE20_REG_PRESS_ABS_LSB            (0xC7)
#define PRESSURE20_REG_PRESS_ABS_MSB            (0xC8)
#define PRESSURE20_REG_PRESS_DELTA_LSB          (0xC9)
#define PRESSURE20_REG_PRESS_DELTA_MSB          (0xCA)
#define PRESSURE20_REG_DEVICE_STATUS            (0xCD)
#define PRESSURE20_REG_I3C_INFO                 (0xCE)
#define PRESSURE20_REG_VERSION                  (0xD3)
#define PRESSURE20_REG_DUMMY                    (0xEE)
#define PRESSURE20_REG_PRESS_DATA_0             (0xFA)
#define PRESSURE20_REG_PRESS_DATA_1             (0xFB)
#define PRESSURE20_REG_PRESS_DATA_2             (0xFC)
#define PRESSURE20_REG_TEMP_DATA_0              (0xFD)
#define PRESSURE20_REG_TEMP_DATA_1              (0xFE)
#define PRESSURE20_REG_TEMP_DATA_2              (0xFF)


/**
 * @brief Pressure 20 TRIM settings.
 * @details Specified TRIM settings of Pressure 20 Click driver.
 */
#define PRESSURE20_TRIM1_MSB_PEFE_OFFSET_MASK   (0x3F)
#define PRESSURE20_TRIM2_LSB_HFOSC_MASK         (0x7F)
#define PRESSURE20_TRIM2_MSB_PEFE_GAIN_MASK     (0x70)
#define PRESSURE20_TRIM2_MSB_BG_PTAT_MASK       (0x0F)

/**
 * @brief Pressure 20 OTP address and command settings.
 * @details Specified OTP address and command settings of Pressure 20 Click driver.
 */
#define PRESSURE20_OTP_ADDRESS_OFFSET           (0xF8)
#define PRESSURE20_OTP_ADDRESS_GAIN             (0xF9)
#define PRESSURE20_OTP_ADDRESS_HFOSC            (0xFA)
#define PRESSURE20_OTP_COMMAND_READ_ACTION      (0x10)


/**
 * @brief Pressure 20 OTP settings.
 * @details Specified OTP settings of Pressure 20 Click driver.
 */
#define PRESSURE20_OTP_MRA_LSB                  (0x04)
#define PRESSURE20_OTP_MRA_MSB                  (0x04)
#define PRESSURE20_OTP_MRB_LSB                  (0x21)
#define PRESSURE20_OTP_MRB_MSB                  (0x20)
#define PRESSURE20_OTP_MR_LSB                   (0x10)
#define PRESSURE20_OTP_MR_MSB                   (0x80)

/**
 * @brief Pressure 20 OTP_CONFIG1 register settings.
 * @details Specified OTP_CONFIG1 register settings of Pressure 20 Click driver.
 */
#define PRESSURE20_OTP_WRITE_SWITCH             (0x02)
#define PRESSURE20_OTP_ENABLE                   (0x01)
#define PRESSURE20_OTP_DISABLE                  (0x00)

/**
 * @brief Pressure 20 OTP_DBG2 register settings.
 * @details Specified OTP_DBG2 register settings of Pressure 20 Click driver.
 */
#define PRESSURE20_OTP_RESET_SET                (0x80)
#define PRESSURE20_OTP_RESET_CLEAR              (0x00)

/**
 * @brief Pressure 20 OTP_STATUS register settings.
 * @details Specified OTP_STATUS register settings of Pressure 20 Click driver.
 */
#define PRESSURE20_BUSY                         (0x01)

/**
 * @brief Pressure 20 OTP_STATUS2 register settings.
 * @details Specified OTP_STATUS2 register settings of Pressure 20 Click driver.
 */
#define PRESSURE20_BOOT_UP_STATUS               (0x01)

/**
 * @brief Pressure 20 MASTER_LOCK register settings.
 * @details Specified MASTER_LOCK register settings of Pressure 20 Click driver.
 */
#define PRESSURE20_MASTER_LOCK                  (0x00)
#define PRESSURE20_MASTER_UNLOCK                (0x1F)

/**
 * @brief Pressure 20 MODE_SELECT register settings.
 * @details Specified MODE_SELECT register settings of Pressure 20 Click driver.
 */
#define PRESSURE20_MEAS_CONFIG_MODE0_ODR_25HZ   (0x00)
#define PRESSURE20_MEAS_CONFIG_MODE1_ODR_120HZ  (0x20)
#define PRESSURE20_MEAS_CONFIG_MODE2_ODR_40HZ   (0x40)
#define PRESSURE20_MEAS_CONFIG_MODE3_ODR_2HZ    (0x60)
#define PRESSURE20_MEAS_CONFIG_MODE4            (0x80)
#define PRESSURE20_FORCED_MEAS_TRIGGER          (0x10)
#define PRESSURE20_MEAS_MODE_TRIGGER            (0x00)
#define PRESSURE20_MEAS_MODE_CONTINUOUS         (0x08)
#define PRESSURE20_POWER_MODE_NORMAL            (0x00)
#define PRESSURE20_POWER_MODE_ACTIVE            (0x04)
#define PRESSURE20_FIFO_READOUT_MODE_PRESS_1ST  (0x00)
#define PRESSURE20_FIFO_READOUT_MODE_TEMP_ONLY  (0x01)
#define PRESSURE20_FIFO_READOUT_MODE_TEMP_1ST   (0x02)
#define PRESSURE20_FIFO_READOUT_MODE_PRESS_ONLY (0x03)

/**
 * )@brief Pressure 20 DEVICE_STATUS register set(ting)s.
 * @details Specified DEVICE_STATUS register set(ting)s of Pressure 20 Click driver.
 */
#define PRESSURE20_MODE_SYNC_STATUS             (0x01)

/**
 * @brief Pressure 20 FIFO_FILL register setting(s.
 * @details Specified FIFO_FILL register setting(s of) Pressure 20 Click driver.
 */
#define PRESSURE20_FIFO_FLUSH                   (0x80)
#define PRESSURE20_FIFO_EMPTY                   (0x40)
#define PRESSURE20_FIFO_FULL                    (0x20)
#define PRESSURE20_FIFO_LEVEL_EMPTY             (0x00)
#define PRESSURE20_FIFO_LEVEL_1                 (0x01)
#define PRESSURE20_FIFO_LEVEL_2                 (0x02)
#define PRESSURE20_FIFO_LEVEL_3                 (0x03)
#define PRESSURE20_FIFO_LEVEL_4                 (0x04)
#define PRESSURE20_FIFO_LEVEL_5                 (0x05)
#define PRESSURE20_FIFO_LEVEL_6                 (0x06)
#define PRESSURE20_FIFO_LEVEL_7                 (0x07)
#define PRESSURE20_FIFO_LEVEL_8                 (0x08)
#define PRESSURE20_FIFO_LEVEL_9                 (0x09)
#define PRESSURE20_FIFO_LEVEL_10                (0x0A)
#define PRESSURE20_FIFO_LEVEL_11                (0x0B)
#define PRESSURE20_FIFO_LEVEL_12                (0x0C)
#define PRESSURE20_FIFO_LEVEL_13                (0x0D)
#define PRESSURE20_FIFO_LEVEL_14                (0x0E)
#define PRESSURE20_FIFO_LEVEL_15                (0x0F)
#define PRESSURE20_FIFO_LEVEL_FULL              (0x10)

/**
 * @brief Pressure 20 INTERRUPT_STATUS register settings.
 * @details Specified INTERRUPT_STATUS register settings of Pressure 20 Click driver.
 */
#define PRESSURE20_INT_STAT_PRESS_DELTA         (0x40)
#define PRESSURE20_INT_STAT_PRESS_ABS           (0x20)
#define PRESSURE20_INT_STAT_FIFO_WMK_LOW        (0x08)
#define PRESSURE20_INT_STAT_FIFO_WMK_HIGH       (0x04)
#define PRESSURE20_INT_STAT_FIFO_UNDERFLOW      (0x02)
#define PRESSURE20_INT_STAT_FIFO_OVERFLOW       (0x01)

/**
 * @brief Pressure 20 INTERRUPT_MASK register settings.
 * @details Specified INTERRUPT_MASK register settings of Pressure 20 Click driver.
 */
#define PRESSURE20_INT_MASK_RESERVED            (0x80)
#define PRESSURE20_INT_MASK_PRESS_DELTA         (0x40)
#define PRESSURE20_INT_MASK_PRESS_ABS           (0x20)
#define PRESSURE20_INT_MASK_FIFO_WMK_LOW        (0x08)
#define PRESSURE20_INT_MASK_FIFO_WMK_HIGH       (0x04)
#define PRESSURE20_INT_MASK_FIFO_UNDERFLOW      (0x02)
#define PRESSURE20_INT_MASK_FIFO_OVERFLOW       (0x01)
#define PRESSURE20_INT_MASK_ALL                 (0xFF)

/**
 * @brief Pressure 20 Device ID value.
 * @details Specified Device ID value of Pressure 20 Click driver.
 */
#define PRESSURE20_DEVICE_ID                    (0x63)

/**
 * @brief Pressure 20 pressure and temperature calculation values.
 * @details Specified pressure and temperature calculation values of Pressure 20 Click driver.
 */
#define PRESSURE20_PRESSURE_RES_RAW             (0x020000ul)
#define PRESSURE20_PRESSURE_RES_MBAR            (400)
#define PRESSURE20_PRESSURE_OFFSET_MBAR         (700)
#define PRESSURE20_TEMPERATURE_RES_RAW          (0x040000ul)
#define PRESSURE20_TEMPERATURE_RES_C            (65)
#define PRESSURE20_TEMPERATURE_OFFSET_C         (25)

/**
 * @brief Pressure 20 SPI Read/Write command.
 * @details Specified SPI Read/Write command of Pressure 20 Click driver.
 */
#define PRESSURE20_SPI_READ_REG                 (0x3C)
#define PRESSURE20_SPI_WRITE_REG                (0x33)

/**
 * @brief Pressure 20 device address setting.
 * @details Specified setting for device slave address selection of
 * Pressure 20 Click driver.
 */
#define PRESSURE20_DEVICE_ADDRESS_0             (0x63)
#define PRESSURE20_DEVICE_ADDRESS_1             (0x64)

#define DUMMY_READ_ADDR                         (0x00)

/**
 * @brief Dummy data.
 * @details Definition of dummy data.
 */
#define DUMMY                                   (0x00)


extern int8_t pressure20_write_register (uint8_t reg, uint8_t *data_in, uint32_t len);
extern int8_t pressure20_read_register (uint8_t reg, uint8_t *data_out, uint32_t len);

extern int8_t pressure20_check_communication ();
extern int8_t pressure20_write_mode_select (uint8_t mode_select);
extern int8_t pressure20_clear_interrupts ();
extern int8_t pressure20_read_otp_data (uint8_t otp_address, uint8_t *data_out);
extern int8_t pressure20_read_register_data (uint8_t reg, uint8_t *data_out, uint32_t len);
extern int8_t pressure20_read_data (float *pressure, float *temperature);

extern int8_t pressure20_default_cfg ();
extern int8_t application_init (void);

extern int8_t dummy_read ();

extern int8_t ICP20100_Sensor_init (void);
extern void ICP_20100_get ();

#endif /* ICP_20100_H_ */
