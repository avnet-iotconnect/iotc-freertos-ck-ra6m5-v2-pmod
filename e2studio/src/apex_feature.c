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
 * File Name    : apex_feature.c
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

/*
 * apex_feature.c
 *
 *  Created on: May 25, 2023
 *      Author: 31342
 */

#include "icm42605.h"

extern int8_t err;
extern uint8_t temp;
extern float32_t Ares;
extern float32_t Gres;


/**********************************************************************************************************************
 * Function Name: init_snsr_typical_config
 * Description  : .
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
int8_t init_snsr_typical_config()
{

    uint8_t Ascal   =   AFS_16G;
    uint8_t Gscal   =   GFS_2000DPS;
    uint8_t Aodr    =   AODR_1000HZ;      /*AODR_50Hz*/
    uint8_t Godr    =   GODR_1000HZ;

    /* get sensor resolutions*/
    Ares = getAres(Ascal);
    Gres = getGres(Gscal);

    /*----------  Initialize sensor in a typical configuration  -----------------*/
    /*select bank 0*/
    err = select_bank_0();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /*set accel scale and data rate */
    temp = 0;

    err = readByte (ICM42605_ACCEL_CONFIG0, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    temp = temp & (~(0xEF));
    temp = (temp | Aodr | (Ascal << 5));

    err = writeByte (ICM42605_ACCEL_CONFIG0, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* set gyro scale and data rate */
    temp = 0;

    err = readByte(ICM42605_GYRO_CONFIG0, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    temp = temp & (~(0xEF));
    temp = (temp | Godr | (Gscal << 5));

    err = writeByte(ICM42605_GYRO_CONFIG0, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /*accel low noise, gyro low noise mode */
    temp = 0;

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

    /*DMP ODR 50 Hz and disable power save mode*/
    temp = 0;

    err = readByte(ICM42605_APEX_CONFIG0, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    temp = temp & (~(0x83));
    temp = temp | 0x2;

    err = writeByte(ICM42605_APEX_CONFIG0, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    delay_ms(1);

    err = select_bank_4();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /*select unitary mounting matrix*/
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
    err = select_bank_0();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return err;
}
/**********************************************************************************************************************
 End of function init_snsr_typical_config
 *********************************************************************************************************************/




/**********************************************************************************************************************
 * Function Name: pedomtr_config
 * Description  : .
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
int8_t pedomtr_config()
{
    /*select bank 4 */
    err = select_bank_4();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /*LOW_ENERGY_AMP_TH_SEL */
    temp = 0;
    err = readByte(ICM42605_APEX_CONFIG1, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    temp = temp & (~(0xF0));
    temp = temp | (0xA << 4);
    err = writeByte(ICM42605_APEX_CONFIG1, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /*PED_AMP_TH_SEL */
    temp = 0;
    err = readByte(ICM42605_APEX_CONFIG2, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    temp = temp & (~(0xF0));
    temp = temp | (0x8 << 4);
    err = writeByte(ICM42605_APEX_CONFIG2, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /*PED_STEP_CNT_TH_SEL (2 step)    (default is 5) */
    temp = 0;
    err = readByte(ICM42605_APEX_CONFIG2, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    temp = temp & (~(0x0F));
    temp = temp | 0x2;
    err = writeByte(ICM42605_APEX_CONFIG2, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /*PED_HI_EN_TH_SEL (01b) */
    temp = 0;
    err = readByte(ICM42605_APEX_CONFIG3, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    temp = temp & (~(0x03));
    temp = temp | 0x1;
    err = writeByte(ICM42605_APEX_CONFIG3, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /*PED_SB_TIMER_TH_SEL (4 samples) */
    temp = 0;
    err = readByte(ICM42605_APEX_CONFIG3, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    temp = temp & (~(0x1C));
    temp = temp | (4<<2);
    err = writeByte(ICM42605_APEX_CONFIG3, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /*PED_STEP_DET_TH_SEL  (2 step)    (default is 2) */
    temp = 0;
    err = readByte(ICM42605_APEX_CONFIG3, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    temp = temp & (~(0xE0));
    temp = temp | (2<<5);
    err = writeByte(ICM42605_APEX_CONFIG3, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /*SENSITIVITY_MODE (0: Low power mode at accelerometer ODR 25Hz;
     * High performance mode at accelerometer ODR ≥ 50Hz) */
    temp = 0;
    err = readByte(ICM42605_APEX_CONFIG9, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    temp = temp & (~(0x01));
    err = writeByte(ICM42605_APEX_CONFIG9, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /*select bank 0 */
    err = select_bank_0();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return err;
}
/**********************************************************************************************************************
 End of function pedomtr_config
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: tilt_config
 * Description  : .
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
int8_t tilt_config()
{
    /*select bank 4 */
    err = select_bank_4();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /*TILT_WAIT_TIME_SEL (4s) */
    temp = 0;
    err = readByte(ICM42605_APEX_CONFIG4, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    temp = temp & (~(0xC0));
    temp = temp | (2<<6);
    err = writeByte(ICM42605_APEX_CONFIG4, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    delay_ms(1);

    /*select bank 0 */
    err = select_bank_0();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return err;
}


int8_t config_INT1()
{
    /*select bank 4 */
    err = select_bank_4();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /*tilt,step,tap, wak/slp detect interrupt routed to INT1 */
    temp = 0;
    err = readByte(ICM42605_INT_SOURCE6, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    temp = (temp | (1<<3) | (1<<5) | (1<<0) | (1<<2) | (1<<1));
    err = writeByte(ICM42605_INT_SOURCE6, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    delay_ms(50);

    /*select bank 0 */
    err = select_bank_0();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return err;
}
/**********************************************************************************************************************
 End of function config_INT1
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: enable_apex_features
 * Description  : .
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
int8_t enable_apex_features()
{
    temp = 0;
    err = readByte(ICM42605_APEX_CONFIG0, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /*enable tilt,step,tap and wak/slp detection */
    temp = (temp | (1<<4) | (1<<5) | (1<<6) | (1<<3));
    err = writeByte(ICM42605_APEX_CONFIG0, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return err;
}
/**********************************************************************************************************************
 End of function enable_apex_features
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: tap_config
 * Description  : .
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
int8_t tap_config()
{

    temp = 0;
    err = readByte(ICM42605_ACCEL_CONFIG1, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    temp = temp & (~(0x18));
    temp = (temp | (2<<3));
    err = writeByte(ICM42605_ACCEL_CONFIG1, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    temp = 0;
    err = readByte(ICM42605_GYRO_ACCEL_CONFIG0, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    temp = temp & (~(0xF0));
    err = writeByte(ICM42605_GYRO_ACCEL_CONFIG0, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    delay_ms(1);

    /*select bank 4 */
    err = select_bank_4();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /*TAP_TMAX=2, TAP_TMIN=3 and TAP_TAVG=3 */
    temp = 0;
    err = readByte(ICM42605_APEX_CONFIG8, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    temp = temp & (~(0x7F));
    temp = temp | 0x5B;
    err = writeByte(ICM42605_APEX_CONFIG8, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /*TAP_MIN_JERK_THR = 17 and TAP_MAX_PEAK_TOL = 2 */
    temp = 0;
    err = readByte(ICM42605_APEX_CONFIG7, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    temp = temp & (~(0xFF));
    temp = temp | (0x11<<2) | 0x2;
    err = writeByte(ICM42605_APEX_CONFIG7, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    delay_ms(1);

    /*select bank 0 */
    err = select_bank_0();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return err;
}
/**********************************************************************************************************************
 End of function tap_config
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: raise_wak_slp_config
 * Description  : .
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
int8_t raise_wak_slp_config()
{
    /*select bank 4 */
    err = select_bank_4();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /*SLEEP_TIME_OUT */
    temp = 0;
    err = readByte(ICM42605_APEX_CONFIG4, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    temp = temp & (~(0x38));
    temp = temp | (0x7<<3);
    err = writeByte(ICM42605_APEX_CONFIG4, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    delay_ms(1);

    /*MOUNTING_MATRIX   (unitary mounting matrix) */
    temp = 0;
    err = readByte(ICM42605_APEX_CONFIG5, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    temp = temp & (~(0x7));
    err = writeByte(ICM42605_APEX_CONFIG5, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    delay_ms(1);

    /*SLEEP_GESTURE_DELAY */
    temp = 0;
    err = readByte(ICM42605_APEX_CONFIG6, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    temp = temp & (~(0x7));
    temp = temp | 0x1;
    err = writeByte(ICM42605_APEX_CONFIG6, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    delay_ms(1);

    /*select bank 0 */
    err = select_bank_0();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return err;
}
/**********************************************************************************************************************
 End of function raise_wak_slp_config
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: set_wom_thrshld
 * Description  : .
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
int8_t set_wom_thrshld()
{
    /*select bank 4 */
    err = select_bank_4();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /*----------- config ---------------*/
    /*WOM_X_TH */
    temp = 0;
    err = readByte(ICM42605_ACCEL_WOM_X_THR, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    temp = 98;
    err = writeByte(ICM42605_ACCEL_WOM_X_THR, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /*WOM_Y_TH */
    temp = 0;
    err = readByte(ICM42605_ACCEL_WOM_Y_THR, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    temp = 98;
    err = writeByte(ICM42605_ACCEL_WOM_Y_THR, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /*WOM_Z_TH */
    temp = 0;
    err = readByte(ICM42605_ACCEL_WOM_Z_THR, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    temp = 98;
    err = writeByte(ICM42605_ACCEL_WOM_Z_THR, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    /*----------- config ---------------*/

    /*select bank 0 */
    err = select_bank_0();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return err;
}
/**********************************************************************************************************************
 End of function set_wom_thrshld
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: wom_config
 * Description  : .
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
int8_t wom_config()
{
    /*select bank 0 */
    err = select_bank_0();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /*WOM interrupt routed to INT1 for all the 3 axes */
    temp = 0;
    err = readByte(ICM42605_INT_SOURCE1, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    temp = temp & (~(0x7));
    temp = (temp | 0x7);
    err = writeByte(ICM42605_INT_SOURCE1, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    delay_ms(50);

    /*Turn on WOM feature by setting WOM_INT_MODE to 0, WOM_MODE to 1, SMD_MODE to 1 */
    temp = 0;
    err = readByte (ICM42605_SMD_CONFIG, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    temp = temp & (~(0xF));
    temp = (temp | 0x5);
    err = writeByte(ICM42605_SMD_CONFIG, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return err;
}
/**********************************************************************************************************************
 End of function wom_config
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: smd_config
 * Description  : .
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
int8_t smd_config()
{
    /*select bank 0 */
    err = select_bank_0();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /*SMD interrupt routed to INT1 */
    temp = 0;
    err = readByte(ICM42605_INT_SOURCE1, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    temp = temp | (1<<3);
    err = writeByte(ICM42605_INT_SOURCE1, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    delay_ms(50);

    /*Turn on SMD feature by setting WOM_INT_MODE to 0, WOM_MODE to 1, SMD_MODE to 2 */
    temp = 0;
    err = readByte(ICM42605_SMD_CONFIG, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    temp = temp & (~(0xF));
    temp = (temp | 0x6);
    err = writeByte(ICM42605_SMD_CONFIG, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return err;
}
/**********************************************************************************************************************
 End of function smd_config
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: icm42605_apex_feature_init
 * Description  : .
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
int8_t icm42605_apex_feature_init()
{
    /*----------  Initialize sensor in a typical configuration  -----------------*/
    err = init_snsr_typical_config();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if (TAP_CHECK == 1)
    /*tap configuration*/
    err = tap_config();
    if (FSP_SUCCESS != err)
    {
        return err;
    }
#endif

    /*set wom thresholds */
    err = set_wom_thrshld();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if (WOM == 1)
    /*WOM configuration and WOM interrupt routed to INT1 */
    err = wom_config();
    if (FSP_SUCCESS != err)
    {
        return err;
    }
#else
    //SMD configuration and SMD interrupt routed to INT1
    err = smd_config();
    if (FSP_SUCCESS != err)
    {
        return err;
    }
#endif

    /*--------------------  Initialize APEX hardware  ---------------------------*/
    /*DMP_MEM_RESET_EN */
    temp = 0;
    err = readByte(ICM42605_SIGNAL_PATH_RESET, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    temp = temp | (1<<5);
    err = writeByte(ICM42605_SIGNAL_PATH_RESET, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    delay_ms(1);

    /*tilt configuration */
    err = tilt_config();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /*pedometer configuration */
    err = pedomtr_config();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /*raise to wake/sleep config */
    err = raise_wak_slp_config();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /*DMP_INIT_EN */
    temp = 0;
    err = readByte(ICM42605_SIGNAL_PATH_RESET, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    temp = temp | (1<<6);

    err = writeByte(ICM42605_SIGNAL_PATH_RESET, &temp, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    delay_ms(50);

/*configure interrupt routed to INT1*/
    err = config_INT1();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

/*enable apex features*/
    err = enable_apex_features();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return err;
}
/**********************************************************************************************************************
 End of function icm42605_apex_feature_init
 *********************************************************************************************************************/




