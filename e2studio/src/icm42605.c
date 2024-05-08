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
 * File Name    : icm42605.c
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

/*
 * icm_42605.c
 *
 *  Created on: 01-May-2023
 *      Author: 31342
 */

#include "icm42605.h"


#define GTOMS2              (9.81f) /* 1G represented as m/s2 */
#define CONVERT_TO_MS2(x,y) ((float32_t)(x = x*y*GTOMS2))

#define CONVERTMS2(x) (float32_t)(GTOMS2/LSBPERG)*x)

float32_t aRes = 0;
float32_t gRes = 0;

int8_t err = 0;
uint8_t temp = 0;

int8_t main (void);

/*offset biases for the accel and gyro*/
float32_t accelBias[3] = {0};
float32_t gyroBias[3] = {0};
float32_t Ares = 0;
float32_t Gres = 0;
uint8_t newICM42605Data = false;
uint8_t ICM42605status = false;

extern int8_t  err;
uint8_t chip_id = 0;
uint8_t int_status = 0;

uint8_t int2_flag = 0;
uint8_t temp1 = 0;

uint8_t int_st = 0;
uint8_t int_st2 = 0;
uint8_t data = 0;

int16_t acl_val[3]  = {0};
int16_t gyr_val[3]  = {0};
int16_t tmpr_val = {0};

uint8_t data_ready = 0;
Mntn_data_t mn_data = {0};

/* Copied from old ICM_20948.c */

char_t g_corracc_x[20];
char_t g_corracc_y[20];
char_t g_corracc_z[20];
char_t g_gval_x[20];
char_t g_gval_y[20];
char_t g_gval_z[20];
char_t g_corgyro_x[20];
char_t g_corgyro_y[20];
char_t g_corgyro_z[20];
char_t g_rawgyro_x[20];
char_t g_rawgyro_y[20];
char_t g_rawgyro_z[20];
char_t g_rawacc_x[20];
char_t g_rawacc_y[20];
char_t g_rawacc_z[20];

char_t g_resultg[20];

xyzfloat g_corraccraw;
xyzfloat g_accraw;
xyzfloat g_gval;
xyzfloat g_gyrraw;
xyzfloat g_gyr;
xyzfloat g_magvalue;


float32_t resultantG;
float32_t resAccel  = 0.0f;
float32_t resGyr    = 0.0f;


/**********************************************************************************************************************
 * Function Name: icm_42605_get
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t icm_42605_get(void)
{
    err = ICM42605_readData(&acl_val, &gyr_val, &tmpr_val);

    if (FSP_SUCCESS != err)
    {
    return ERROR;
    }

    g_corraccraw.x = acl_val[0] * 1.0;
    g_corraccraw.y = acl_val[1] * 1.0;
    g_corraccraw.z = acl_val[2] * 1.0;

    /* get sensor resolutions */
    resAccel = getAres(AFS_16G);
    resGyr = getGres(GFS_2000DPS);


    g_corraccraw.x *= resAccel;
    g_corraccraw.y *= resAccel;
    g_corraccraw.z *= resAccel;

    g_gyr.x = gyr_val[0] * 1.0 * resGyr;
    g_gyr.y = gyr_val[1] * 1.0 * resGyr;
    g_gyr.z = gyr_val[2] * 1.0 * resGyr;

return err;
}
/**********************************************************************************************************************
 End of function icm_42605_get
 *********************************************************************************************************************/




/**********************************************************************************************************************
 * Function Name: icm42605_sensor_init
 * Description  : .
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
int8_t icm42605_sensor_init()
{
    /* not required - will already have been done in rmcomdevice_init_icm() */
    err =  getChipID(&chip_id);

    if ((DEVICE_ID != chip_id) && (FSP_SUCCESS != err))
    {
    return ERROR;
    }

    err = ICM42605_reset();
    if (FSP_SUCCESS != err)
    {
    return err;
    }

    err = icm42605_apex_feature_init();
    if (FSP_SUCCESS != err)
    {
    return err;
    }

    err = ICM42605_offsetBias(accelBias, gyroBias);
    if (FSP_SUCCESS != err)
    {
    return err;
    }

    /*clear int_status register */
    err = ICM42605_status(&int_status);
    if (FSP_SUCCESS != err)
    {
    return err;
    }

return err;
}
/**********************************************************************************************************************
 End of function icm42605_sensor_init
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: writeByte
 * Description  : .
 * Arguments    : reg
 *              : data_in
 *              : len
 * Return Value : .
 *********************************************************************************************************************/
int8_t writeByte(uint8_t reg, uint8_t *data_in, uint32_t len)
{

    err = rmcom_i2c_w(reg, data_in, len);
    return err;
}
/**********************************************************************************************************************
 End of function writeByte
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: readByte
 * Description  : .
 * Arguments    : reg
 *              : data_out
 *              : len
 * Return Value : .
 *********************************************************************************************************************/
uint8_t readByte(uint8_t reg, uint8_t *data_out, uint32_t len)
{

    err = rmcom_i2c_r(reg, data_out, len);
    return err;
}
/**********************************************************************************************************************
 End of function readByte
 *********************************************************************************************************************/



int8_t getChipID(uint8_t *val)
{

    err = readByte(ICM42605_WHO_AM_I, val, 1);
    return err;
}


void delay_ms(uint32_t delay)
{
    R_BSP_SoftwareDelay(delay, BSP_DELAY_UNITS_MILLISECONDS);
}


/**********************************************************************************************************************
 * Function Name: getAres
 * Description  : .
 * Argument     : Ascale
 * Return Value : .
 *********************************************************************************************************************/
float32_t getAres(uint8_t Ascale)
{
    switch (Ascale)
    {
        /* Possible accelerometer scales (and their register bit settings) are: */
        /* 2 Gs (00), 4 Gs (01), 8 Gs (10), and 16 Gs  (11). */
        case AFS_2G:
            aRes = 2.0f / 32768.0f;
            break;
        case AFS_4G:
            aRes = 4.0f / 32768.0f;
            break;
        case AFS_8G:
            aRes = 8.0f / 32768.0f;
            break;
        case AFS_16G:
            aRes = 16.0f / 32768.0f;
            break;
        default:
            break;
    }

return aRes;
}
/**********************************************************************************************************************
 End of function getAres
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: getGres
 * Description  : .
 * Argument     : Gscale
 * Return Value : .
 *********************************************************************************************************************/
float32_t getGres(uint8_t Gscale)
{
    switch (Gscale)
    {
        case GFS_15_125DPS:
            gRes = 15.125f / 32768.0f;
            break;
        case GFS_31_25DPS:
            gRes = 31.25f / 32768.0f;
            break;
        case GFS_62_5DPS:
            gRes = 62.5f / 32768.0f;
            break;
        case GFS_125DPS:
            gRes = 125.0f / 32768.0f;
            break;
        case GFS_250DPS:
            gRes = 250.0f / 32768.0f;
            break;
        case GFS_500DPS:
            gRes = 500.0f / 32768.0f;
            break;
        case GFS_1000DPS:
            gRes = 1000.0f / 32768.0f;
            break;
        case GFS_2000DPS:
            gRes = 2000.0f / 32768.0f;
            break;
        default:
            break;
    }

return gRes;
}


/**********************************************************************************************************************
 * Function Name: ICM42605_reset
 * Description  : .
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
int8_t ICM42605_reset()
{
    /*reset device */
    err = readByte(ICM42605_DEVICE_CONFIG, &temp, 1);
    if (FSP_SUCCESS != err)
    {
    return err;
    }

    temp = temp | 0x01;
    err = writeByte(ICM42605_DEVICE_CONFIG, &temp, 1); // Set bit 0 to 1 to reset ICM42605
    if (FSP_SUCCESS != err)
    {
    return err;
    }

    delay_ms(50); // Wait for all registers to reset

return err;
}
/**********************************************************************************************************************
 End of function ICM42605_reset
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: ICM42605_status
 * Description  : .
 * Argument     : val
 * Return Value : .
 *********************************************************************************************************************/
int8_t ICM42605_status(uint8_t *val)
{
    /*int8_t err = 0; */
    err = readByte(ICM42605_INT_STATUS, val, 1);
    if (FSP_SUCCESS != err)
    {
    return err;
    }

return err;
}
/**********************************************************************************************************************
 End of function ICM42605_status
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: select_bank_0
 * Description  : .
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
int8_t select_bank_0()
{
    temp = 0;
    err = readByte(ICM42605_REG_BANK_SEL, &temp, 1);
    if (FSP_SUCCESS != err)
    {
    return err;
    }
    temp = temp & (~(0x07));
    err = writeByte(ICM42605_REG_BANK_SEL, &temp, 1); // select Bank 0
    if (FSP_SUCCESS != err)
    {
    return err;
    }

return err;
}
/**********************************************************************************************************************
 End of function select_bank_0
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: select_bank_4
 * Description  : .
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
int8_t select_bank_4()
{
    temp = 0;
    err = readByte(ICM42605_REG_BANK_SEL, &temp, 1);
    if (FSP_SUCCESS != err)
    {
    return err;
    }
    temp = (temp | 0x04);
    err = writeByte(ICM42605_REG_BANK_SEL, &temp, 1); // select Bank 4
    if (FSP_SUCCESS != err)
    {
    return err;
    }

return err;
}
/**********************************************************************************************************************
 End of function select_bank_4
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: ICM42605_init
 * Description  : .
 * Arguments    : Ascale
 *              : Gscale
 *              : AODR
 *              : GODR
 * Return Value : .
 *********************************************************************************************************************/
int8_t ICM42605_init(uint8_t Ascale, uint8_t Gscale, uint8_t AODR, uint8_t GODR)
{
    /*enable gyro and accel in low noise mode */
    err = readByte(ICM42605_PWR_MGMT0, &temp, 1); // make sure not to disturb reserved bit values
    if (FSP_SUCCESS != err)
    {
    return err;
    }
    temp = (temp | 0x0F);
    err = writeByte(ICM42605_PWR_MGMT0, &temp, 1);
    if (FSP_SUCCESS != err)
    {
    return err;
    }

    /*set gyro scale and data rate */
    temp = 0;
    err = readByte(ICM42605_GYRO_CONFIG0, &temp, 1);
    if (FSP_SUCCESS != err)
    {
    return err;
    }
    temp = temp & (~(0xEF));
    temp = (temp | GODR | (Gscale << 5));
    err = writeByte(ICM42605_GYRO_CONFIG0, &temp, 1);
    if (FSP_SUCCESS != err)
    {
    return err;
    }

    /* set accel scale and data rate */
    temp = 0;
    err = readByte(ICM42605_ACCEL_CONFIG0, &temp, 1);
    if (FSP_SUCCESS != err)
    {
    return err;
    }
    temp = temp & (~(0xEF));
    temp = (temp | AODR | (Ascale << 5));
    err = writeByte(ICM42605_ACCEL_CONFIG0, &temp, 1);
    if (FSP_SUCCESS != err)
    {
    return err;
    }

    /* set temperature sensor low pass filter to 5Hz, use first order gyro filter */
    temp = 0;
    err = readByte(ICM42605_GYRO_CONFIG1, &temp, 1);
    if (FSP_SUCCESS != err)
    {
    return err;
    }
    temp = temp & (~(0xEF));
    temp = (temp | 0xC0);    //(temp | 0xD0);
    err = writeByte(ICM42605_GYRO_CONFIG1, &temp, 1);
    if (FSP_SUCCESS != err)
    {
    return err;
    }

    /* set both interrupts active high, push-pull, pulsed */
    temp = 0;
    err = readByte(ICM42605_INT_CONFIG, &temp, 1);
    if (FSP_SUCCESS != err)
    {
    return err;
    }
    temp = (temp | 0x18 | 0x03);
    err = writeByte(ICM42605_INT_CONFIG, &temp, 1);
    if (FSP_SUCCESS != err)
    {
    return err;
    }

    /* set bit 4 to zero for proper function of INT1 and INT2 */
    temp = 0;
    err = readByte(ICM42605_INT_CONFIG1, &temp, 1);
    if (FSP_SUCCESS != err)
    {
    return err;
    }
    temp = temp & (~(0x10));
    err = writeByte(ICM42605_INT_CONFIG1, &temp, 1);
    if (FSP_SUCCESS != err)
    {
    return err;
    }

    /*route data ready interrupt to INT1 */
    temp = 0;
    err = readByte(ICM42605_INT_SOURCE0, &temp, 1);
    if (FSP_SUCCESS != err)
    {
    return err;
    }
    temp = (temp | 0x08);
    err = writeByte(ICM42605_INT_SOURCE0, &temp, 1);
    if (FSP_SUCCESS != err)
    {
    return err;
    }



    /* Select Bank 4 */
    temp = 0;
    err = readByte(ICM42605_REG_BANK_SEL, &temp, 1);
    if (FSP_SUCCESS != err)
    {
    return err;
    }
    temp = (temp | 0x04);

    err  = writeByte(ICM42605_REG_BANK_SEL, &temp, 1);
    if (FSP_SUCCESS != err)
    {
    return err;
    }

    /* select unitary mounting matrix */
    temp = 0;
    err = readByte(ICM42605_APEX_CONFIG5, &temp, 1);
    if (FSP_SUCCESS != err)
    {
    return err;
    }
    temp = temp & (~(0x07));
    err = writeByte(ICM42605_APEX_CONFIG5, &temp, 1);
    if (FSP_SUCCESS != err)
    {
    return err;
    }

    /* select Bank 0 */
    temp = 0;
    err = readByte(ICM42605_REG_BANK_SEL, &temp, 1);
    if (FSP_SUCCESS != err)
    {
    return err;
    }
    temp = temp & (~(0x07));
    err = writeByte(ICM42605_REG_BANK_SEL, &temp, 1);
    if (FSP_SUCCESS != err)
    {
    return err;
    }

return err;
}
/**********************************************************************************************************************
 End of function ICM42605_init
 *********************************************************************************************************************/


int16_t acl_val_ofst[3] = {0};
int16_t gy_val_ofst[3] = {0};
int16_t tempr_ofst = 0;
int32_t sum[7] = {0, 0, 0, 0, 0, 0, 0};

/**********************************************************************************************************************
 * Function Name: ICM42605_offsetBias
 * Description  : .
 * Arguments    : a_ofst
 *              : g_ofst
 * Return Value : .
 *********************************************************************************************************************/
int8_t ICM42605_offsetBias(float32_t *a_ofst, float32_t *g_ofst)
{

    for (int_t ii = 0; ii < 128; ii++)
    {
        err = ICM42605_readData(&acl_val_ofst[0], &gy_val_ofst[0], &tempr_ofst);
        if (FSP_SUCCESS != err)
        {
        return err;
        }

        sum[1] += acl_val_ofst[0];
        sum[2] += acl_val_ofst[1];
        sum[3] += acl_val_ofst[2];
        sum[4] += gy_val_ofst[0];
        sum[5] += gy_val_ofst[1];
        sum[6] += gy_val_ofst[2];
        delay_ms(50);
    }

    a_ofst[0] = (sum[1] * aRes) / 128.0f;
    a_ofst[1] = (sum[2] * aRes) / 128.0f;
    a_ofst[2] = (sum[3] * aRes) / 128.0f;

    g_ofst[0] = (sum[4] * gRes) / 128.0f;
    g_ofst[1] = (sum[5] * gRes) / 128.0f;
    g_ofst[2] = (sum[6] * gRes) / 128.0f;

    if (a_ofst[0] > 0.8f)
    {
        a_ofst[0] -= 1.0f; // Remove gravity from the x-axis accelerometer bias calculation
    }
    if (a_ofst[0] < (-0.8f))
    {
        a_ofst[0] += 1.0f; // Remove gravity from the x-axis accelerometer bias calculation
    }

    if (a_ofst[1] > 0.8f)
    {
        a_ofst[1] -= 1.0f; // Remove gravity from the y-axis accelerometer bias calculation
    }
    if (a_ofst[1] < (-0.8f))
    {
        a_ofst[1] += 1.0f; // Remove gravity from the y-axis accelerometer bias calculation
    }

    if (a_ofst[2] > 0.8f)
    {
        a_ofst[2] -= 1.0f; // Remove gravity from the z-axis accelerometer bias calculation
    }
    if (a_ofst[2] < (-0.8f))
    {
        a_ofst[2] += 1.0f; // Remove gravity from the z-axis accelerometer bias calculation
    }


return err;
}

uint8_t rawData[14]     = {0};
uint8_t acc_x[2]        = {0};
uint8_t acc_y[2]        = {0};
uint8_t acc_z[2]        = {0};
int16_t adj_to_signed   = {0};
uint8_t acc_config      = 0U;


/**********************************************************************************************************************
 * Function Name: ICM42605_readData
 * Description  : .
 * Arguments    : accel_val
 *              : gyro_val
 *              : tempr
 * Return Value : .
 *********************************************************************************************************************/
int8_t ICM42605_readData(int16_t *accel_val, int16_t *gyro_val, int16_t *tempr)
{
    /* Read the 14 raw data registers into data array */
    err = readByte(ICM42605_TEMP_DATA1, &rawData[0], 14);
    if (FSP_SUCCESS != err)
    {
    return err;
    }

    /* Turn the MSB and LSB into a signed 16-bit value */
    /*temperature value */
    *tempr = ((int16_t)rawData[0] << 8) | rawData[1] ;
    /*accel x value */
    accel_val[0] = ((int16_t)rawData[2] << 8) | rawData[3] ;
    /*accel y value */
    accel_val[1] = ((int16_t)rawData[4] << 8) | rawData[5] ;
    /*accel z value */
    accel_val[2] = ((int16_t)rawData[6] << 8) | rawData[7] ;

    /*gyro x value */
    gyro_val[0] = ((int16_t)rawData[8] << 8) | rawData[9] ;
    /*gyro y value */
    gyro_val[1] = ((int16_t)rawData[10] << 8) | rawData[11] ;
    /*gyro z value */
    gyro_val[2] = ((int16_t)rawData[12] << 8) | rawData[13] ;

return err;
}



/**********************************************************************************************************************
 * Function Name: icm_int1_isr
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void icm_int1_isr(external_irq_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    /* Data ready */
}
/**********************************************************************************************************************
 End of function icm_int1_isr
 *********************************************************************************************************************/




/**********************************************************************************************************************
 * Function Name: icm_int2_isr
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void icm_int2_isr(external_irq_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    /* Data ready */
}
/**********************************************************************************************************************
 End of function icm_int2_isr
 *********************************************************************************************************************/




