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
 * File Name    : ICM42605.h
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/
/* 07/13/2019 Copyright Tlera Corporation

    Created by Kris Winer

  This sketch uses SDA/SCL on pins 21/20 (Dragonfly default), respectively,
  and it uses the Dragonfly STM32L476RE Breakout Board.
  The ICM42605 is a sensor hub with embedded accel and gyro,
  here used as 6 DoF in a 9 DoF absolute orientation solution.

  Library may be used freely and without limit with attribution.

*/

#ifndef icm42605_h
#define icm42605_h

#include <stdint.h>
#include "r_typedefs.h"
#include "rmci2c.h"

#define  DEVICE_ID       (0x42)
#define  ERROR           (-1)
#define  TAP_DET_INT     (0)
#define  TILT_DET_INT    (3)
#define  STEP_DET_INT    (5)
#define  WAKE_DET_INT    (2)
#define  SLEEP_DET_INT   (1)
#define  TAP_CHECK       (1)
#define  WOM             (1)

typedef struct
{
    float64_t x;
    float64_t y;
    float64_t z;
}xyzfloat;

typedef struct
{
    uint8_t tap_dtn;
    uint8_t tap_num;
    uint8_t tap_axis;
    uint8_t tap_dir;
}Tap_t;

typedef struct
{
    uint8_t  wom_int_x;        //Wake on Motion Interrupt on X-axis
    uint8_t  wom_int_y;        //Wake on Motion Interrupt on Y-axis
    uint8_t  wom_int_z;        //Wake on Motion Interrupt on Z-axis
}WOM_t;


/* Pedometer output */
typedef struct APEX_DATA_STEP_ACTIVITY {
    uint16_t step_cnt; /* Number of steps taken */
    uint8_t  step_cadence; /* Walk/run cadence in number of samples. Format is u6.2.*/
    uint8_t  activity_class; /* Detected activity unknown (0), walk (1) or run (2) */
} APEX_DATA_STEP_ACTIVITY_t;


typedef struct
{
    uint8_t                     tilt_dtn;    //1: detected, 0: not detected
    uint8_t                     step_dtn;    //step detection
    APEX_DATA_STEP_ACTIVITY_t   pedmtr;      //pedometer data
    Tap_t                       tap;         //tap detection
    uint8_t                     sleep_dtn;   //sleep detect
    uint8_t                     wake_dtn;    //wake detect
    WOM_t                       wom;         //wake on motion
    uint8_t                     sm_dtn;      //significant motion detection
}Mntn_data_t;


/* ICM42605 registers
https://store.invensense.com/datasheets/invensense/DS-ICM-42605v1-2.pdf
*/
/* Bank 0 */
#define ICM42605_DEVICE_CONFIG             (0x11)
#define ICM42605_DRIVE_CONFIG              (0x13)
#define ICM42605_INT_CONFIG                (0x14)
#define ICM42605_FIFO_CONFIG               (0x16)
#define ICM42605_TEMP_DATA1                (0x1D)
#define ICM42605_TEMP_DATA0                (0x1E)
#define ICM42605_ACCEL_DATA_X1             (0x1F)
#define ICM42605_ACCEL_DATA_X0             (0x20)
#define ICM42605_ACCEL_DATA_Y1             (0x21)
#define ICM42605_ACCEL_DATA_Y0             (0x22)
#define ICM42605_ACCEL_DATA_Z1             (0x23)
#define ICM42605_ACCEL_DATA_Z0             (0x24)
#define ICM42605_GYRO_DATA_X1              (0x25)
#define ICM42605_GYRO_DATA_X0              (0x26)
#define ICM42605_GYRO_DATA_Y1              (0x27)
#define ICM42605_GYRO_DATA_Y0              (0x28)
#define ICM42605_GYRO_DATA_Z1              (0x29)
#define ICM42605_GYRO_DATA_Z0              (0x2A)
#define ICM42605_TMST_FSYNCH               (0x2B)
#define ICM42605_TMST_FSYNCL               (0x2C)
#define ICM42605_INT_STATUS                (0x2D)
#define ICM42605_FIFO_COUNTH               (0x2E)
#define ICM42605_FIFO_COUNTL               (0x2F)
#define ICM42605_FIFO_DATA                 (0x30)
#define ICM42605_APEX_DATA0                (0x31)
#define ICM42605_APEX_DATA1                (0x32)
#define ICM42605_APEX_DATA2                (0x33)
#define ICM42605_APEX_DATA3                (0x34)
#define ICM42605_APEX_DATA4                (0x35)
#define ICM42605_APEX_DATA5                (0x36)
#define ICM42605_INT_STATUS2               (0x37)
#define ICM42605_INT_STATUS3               (0x38)
#define ICM42605_SIGNAL_PATH_RESET         (0x4B)
#define ICM42605_INTF_CONFIG0              (0x4C)
#define ICM42605_INTF_CONFIG1              (0x4D)
#define ICM42605_PWR_MGMT0                 (0x4E)
#define ICM42605_GYRO_CONFIG0              (0x4F)
#define ICM42605_ACCEL_CONFIG0             (0x50)
#define ICM42605_GYRO_CONFIG1              (0x51)
#define ICM42605_GYRO_ACCEL_CONFIG0        (0x52)
#define ICM42605_ACCEL_CONFIG1             (0x53)
#define ICM42605_TMST_CONFIG               (0x54)
#define ICM42605_APEX_CONFIG0              (0x56)
#define ICM42605_SMD_CONFIG                (0x57)
#define ICM42605_FIFO_CONFIG1              (0x5F)
#define ICM42605_FIFO_CONFIG2              (0x60)
#define ICM42605_FIFO_CONFIG3              (0x61)
#define ICM42605_FSYNC_CONFIG              (0x62)
#define ICM42605_INT_CONFIG0               (0x63)
#define ICM42605_INT_CONFIG1               (0x64)
#define ICM42605_INT_SOURCE0               (0x65)
#define ICM42605_INT_SOURCE1               (0x66)
#define ICM42605_INT_SOURCE3               (0x68)
#define ICM42605_INT_SOURCE4               (0x69)
#define ICM42605_FIFO_LOST_PKT0            (0x6C)
#define ICM42605_FIFO_LOST_PKT1            (0x6D)
#define ICM42605_SELF_TEST_CONFIG          (0x70)
#define ICM42605_WHO_AM_I                  (0x75)
#define ICM42605_REG_BANK_SEL              (0x76)

/* Bank 1 */
#define ICM42605_SENSOR_CONFIG0            (0x03)
#define ICM42605_GYRO_CONFIG_STATIC2       (0x0B)
#define ICM42605_GYRO_CONFIG_STATIC3       (0x0C)
#define ICM42605_GYRO_CONFIG_STATIC4       (0x0D)
#define ICM42605_GYRO_CONFIG_STATIC5       (0x0E)
#define ICM42605_GYRO_CONFIG_STATIC6       (0x0F)
#define ICM42605_GYRO_CONFIG_STATIC7       (0x10)
#define ICM42605_GYRO_CONFIG_STATIC8       (0x11)
#define ICM42605_GYRO_CONFIG_STATIC9       (0x12)
#define ICM42605_GYRO_CONFIG_STATIC10      (0x13)
#define ICM42605_XG_ST_DATA                (0x5F)
#define ICM42605_YG_ST_DATA                (0x60)
#define ICM42605_ZG_ST_DATA                (0x61)
#define ICM42605_TMSTVAL0                  (0x62)
#define ICM42605_TMSTVAL1                  (0x63)
#define ICM42605_TMSTVAL2                  (0x64)
#define ICM42605_INTF_CONFIG4              (0x7A)
#define ICM42605_INTF_CONFIG5              (0x7B)
#define ICM42605_INTF_CONFIG6              (0x7C)

/* Bank 2 */
#define ICM42605_ACCEL_CONFIG_STATIC2      (0x03)
#define ICM42605_ACCEL_CONFIG_STATIC3      (0x04)
#define ICM42605_ACCEL_CONFIG_STATIC4      (0x05)
#define ICM42605_XA_ST_DATA                (0x3B)
#define ICM42605_YA_ST_DATA                (0x3C)
#define ICM42605_ZA_ST_DATA                (0x3D)

/* Bank 4 */
#define ICM42605_GYRO_ON_OFF_CONFIG        (0x0E)
#define ICM42605_APEX_CONFIG1              (0x40)
#define ICM42605_APEX_CONFIG2              (0x41)
#define ICM42605_APEX_CONFIG3              (0x42)
#define ICM42605_APEX_CONFIG4              (0x43)
#define ICM42605_APEX_CONFIG5              (0x44)
#define ICM42605_APEX_CONFIG6              (0x45)
#define ICM42605_APEX_CONFIG7              (0x46)
#define ICM42605_APEX_CONFIG8              (0x47)
#define ICM42605_APEX_CONFIG9              (0x48)
#define ICM42605_ACCEL_WOM_X_THR           (0x4A)
#define ICM42605_ACCEL_WOM_Y_THR           (0x4B)
#define ICM42605_ACCEL_WOM_Z_THR           (0x4C)
#define ICM42605_INT_SOURCE6               (0x4D)
#define ICM42605_INT_SOURCE7               (0x4E)
#define ICM42605_INT_SOURCE8               (0x4F)
#define ICM42605_INT_SOURCE9               (0x50)
#define ICM42605_INT_SOURCE10              (0x51)
#define ICM42605_OFFSET_USER0              (0x77)
#define ICM42605_OFFSET_USER1              (0x78)
#define ICM42605_OFFSET_USER2              (0x79)
#define ICM42605_OFFSET_USER3              (0x7A)
#define ICM42605_OFFSET_USER4              (0x7B)
#define ICM42605_OFFSET_USER5              (0x7C)
#define ICM42605_OFFSET_USER6              (0x7D)
#define ICM42605_OFFSET_USER7              (0x7E)
#define ICM42605_OFFSET_USER8              (0x7F)


#define AFS_2G  (0x03)
#define AFS_4G  (0x02)
#define AFS_8G  (0x01)
#define AFS_16G (0x00)  /* default */

#define GFS_2000DPS   (0x00) /* default */
#define GFS_1000DPS   (0x01)
#define GFS_500DPS    (0x02)
#define GFS_250DPS    (0x03)
#define GFS_125DPS    (0x04)
#define GFS_62_5DPS   (0x05)
#define GFS_31_25DPS  (0x06)
#define GFS_15_125DPS (0x07)

#define AODR_8000HZ   (0x03)
#define AODR_4000HZ   (0x04)
#define AODR_2000HZ   (0x05)
#define AODR_1000HZ   (0x06) /* default */
#define AODR_200HZ    (0x07)
#define AODR_100HZ    (0x08)
#define AODR_50HZ     (0x09)
#define AODR_25HZ     (0x0A)
#define AODR_12_5HZ   (0x0B)
#define AODR_6_25HZ   (0x0C)
#define AODR_3_125HZ  (0x0D)
#define AODR_1_5625HZ (0x0E)
#define AODR_500HZ    (0x0F)

#define GODR_8000HZ  (0x03)
#define GODR_4000HZ  (0x04)
#define GODR_2000HZ  (0x05)
#define GODR_1000HZ  (0x06) /* default */
#define GODR_200HZ   (0x07)
#define GODR_100HZ   (0x08)
#define GODR_50HZ    (0x09)
#define GODR_25HZ    (0x0A)
#define GODR_12_5HZ  (0x0B)
#define GODR_500HZ   (0x0F)


extern int8_t    writeByte (uint8_t reg, uint8_t *data_in, uint32_t len);
extern uint8_t   readByte (uint8_t reg, uint8_t *data_out, uint32_t len);
extern int8_t    getChipID (uint8_t *val);
extern void      delay_ms (uint32_t delay);
extern float32_t getAres (uint8_t Ascale);
extern float32_t getGres (uint8_t Gscale);
extern int8_t    ICM42605_reset ();
extern int8_t    ICM42605_status (uint8_t *val);
extern int8_t    ICM42605_init (uint8_t Ascale, uint8_t Gscale, uint8_t AODR, uint8_t GODR);
extern int8_t    ICM42605_offsetBias (float32_t *a_ofst, float32_t *g_ofst);
extern int8_t    ICM42605_readData (int16_t *accel_val, int16_t *gyro_val, int16_t *tempr);
extern void      display_data (float32_t *aVal, float32_t *gVal, float32_t tmpVal);
extern int8_t    ICM42605_setup ();
extern void      INT1_handler ();
extern int8_t    icm42605_loop ();
extern void      display_motn_data (Mntn_data_t *mn_dtn);
extern int8_t    select_bank_0 ();
extern int8_t    select_bank_4 ();

extern int8_t init_snsr_typical_config ();
extern int8_t config_INT1 ();
extern int8_t tilt_config ();
extern int8_t pedomtr_config ();
extern int8_t raise_wak_slp_config ();
extern int8_t tap_config ();
extern int8_t enable_apex_features ();
extern int8_t read_int_status2_reg (Mntn_data_t *mn_dt);
extern int8_t read_int_status3_reg (Mntn_data_t *mn_dt);

extern int8_t get_xyz_values ();
extern int8_t set_wom_thrshld ();
extern int8_t wom_config ();
extern int8_t smd_config ();
extern int8_t ICM42605_apex_feature_init ();

#endif
