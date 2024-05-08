/***********************************************************************************************************************
 * File Name    : rmci2c.c
 * Description  : Contains data structures and function definitions
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************
 * This code was created by Dan Allen, 2020, Renesas Electronics America. It is intended for demonstration purposes only
 * not for production. No performance or suitability for any purpose including medical devices is guaranteed or claimed.
 * For questions and inquiries visit Renesas.com
 **********************************************************************************************************************/

#include <icp_20100.h>
#include "rmci2c.h"
#include "sensor_thread.h"
#include "icp_20100_thread.h"
#include "common_utils.h"

#define I2C_TRANSMISSION_IN_PROGRESS    (0)
#define I2C_TRANSMISSION_COMPLETE       (1)
#define I2C_TRANSMISSION_ABORTED        (2)

static uint8_t  transmit_complete_flag;
static uint8_t  ICP_transmit_complete_flag;

/* variable to keep initial pressure as reference point */
float32_t           reference_pressure = 0.0;



/**********************************************************************************************************************
 * Function Name: RmComDevice_init
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void RmComDevice_init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = g_comms_i2c_device4.p_api->open(g_comms_i2c_device4.p_ctrl,g_comms_i2c_device4.p_cfg);
    if(FSP_SUCCESS==err)
    {
        APP_PRINT("\r\nICP20100 open sensor instance successful: %d\n", err);
    }
    else
    {
        APP_PRINT("\r\nICP20100 open sensor instance failed: %d\n", err);
    }
}



/**********************************************************************************************************************
 * Function Name: rmcomdevice_init_icm
 * Description  : Initialization of communication layer for ICM
 * Return Value : .
 *********************************************************************************************************************/
void rmcomdevice_init_icm(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = g_comms_i2c_device5.p_api->open(g_comms_i2c_device5.p_ctrl,g_comms_i2c_device5.p_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_PRINT("** I2C open failed: %u\r\n",err);
    }
}
/**********************************************************************************************************************
 End of function rmcomdevice_init_icm
 *********************************************************************************************************************/




/**********************************************************************************************************************
 * Function Name: rmcom_i2c_r
 * Description  : .
 * Arguments    : reg
 *              : val
 *              : num
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t rmcom_i2c_r(uint8_t reg, uint8_t *val, uint8_t num)
{
    uint16_t timeout = 1000;
    fsp_err_t err;
    transmit_complete_flag = I2C_TRANSMISSION_IN_PROGRESS;
    rm_comms_write_read_params_t write_read_params;

    write_read_params.p_src         =   &reg;
    write_read_params.src_bytes     =   1;
    write_read_params.p_dest        =   val;
    write_read_params.dest_bytes    =   num;

    err = g_comms_i2c_device5.p_api->writeRead(g_comms_i2c_device5.p_ctrl,write_read_params);
    if (FSP_SUCCESS == err)
    {
        while (I2C_TRANSMISSION_IN_PROGRESS == transmit_complete_flag)
        {
            if (0 == (--timeout))
            {
                break;
            }
            delay_microseconds(1);
        }

        transmit_complete_flag = I2C_TRANSMISSION_IN_PROGRESS;
    }

    if (I2C_TRANSMISSION_ABORTED == transmit_complete_flag)
    {
        err = FSP_ERR_ABORTED;
    }
    return err;
}
/**********************************************************************************************************************
 End of function rmcom_i2c_r
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: rmcom_i2c_w
 * Description  : .
 * Arguments    : register address, buffer pointer to store read data and number of bytes to read
 *              : val
 *              : num
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t rmcom_i2c_w(uint8_t reg, uint8_t *val, uint8_t num)
{
    uint16_t timeout = 1000;

    fsp_err_t       err;
    static uint8_t  data[50];

    data[0] = reg;
    memcpy(&(data[1]), val, (size_t)num);
    transmit_complete_flag = I2C_TRANSMISSION_IN_PROGRESS;

    err = g_comms_i2c_device5.p_api->write(g_comms_i2c_device5.p_ctrl,data, (uint32_t)(num + 1));
    if (FSP_SUCCESS == err)
    {
        while (I2C_TRANSMISSION_IN_PROGRESS == transmit_complete_flag)
        {
            if (0 == (--timeout))
            {
                break;
            }
            delay_microseconds(1);
        }

        transmit_complete_flag = I2C_TRANSMISSION_IN_PROGRESS;
    }

    if (I2C_TRANSMISSION_ABORTED == transmit_complete_flag)
    {
        err = FSP_ERR_ABORTED;
    }
    return err;
}
/**********************************************************************************************************************
 End of function rmcom_i2c_w
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: icm_comms_i2c_callback
 * Description  : I2C callback for ICM sensor
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void icm_comms_i2c_callback(rm_comms_callback_args_t *p_args)
{
    if (RM_COMMS_EVENT_OPERATION_COMPLETE == p_args->event)
    {
        transmit_complete_flag = I2C_TRANSMISSION_COMPLETE;
    }
    else
    {
        transmit_complete_flag = I2C_TRANSMISSION_ABORTED;
    }
}
/**********************************************************************************************************************
 End of function icm_comms_i2c_callback
 *********************************************************************************************************************/


/* NEW 11/07/2023 - Communicating with the ICP20100 sensor using register read/writes, not commands.
 * Functions are basically the same as those to communicate with the ICM sensor except a different i2c instance.
 * TODO : use a common function for i2c_device4 and 5 and pass a pointer or just use a macro to differentiate.
 */


/**********************************************************************************************************************
 * Function Name: icp_rmcom_i2c_r
 * Description  : .
 * Arguments    : reg register address, buffer pointer to store read data and number of to read
 *              : val
 *              : num
 * Return Value : FSP_SUCCESS         Upon successful open and start of timer
 *                Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 *********************************************************************************************************************/
fsp_err_t icp_rmcom_i2c_r(uint8_t reg, uint8_t *val, uint8_t num)
{
    uint16_t timeout = 1000;

    fsp_err_t err;
    transmit_complete_flag       = I2C_TRANSMISSION_IN_PROGRESS;
    rm_comms_write_read_params_t write_read_params;

    write_read_params.p_src      = &reg;
    write_read_params.src_bytes  = 1;
    write_read_params.p_dest     = val;
    write_read_params.dest_bytes = num;

    err = g_comms_i2c_device4.p_api->writeRead(g_comms_i2c_device4.p_ctrl, write_read_params);
    if (FSP_SUCCESS == err)
    {
        while (I2C_TRANSMISSION_IN_PROGRESS == transmit_complete_flag)
        {
            if (0 == (--timeout))
            {
                break;
            }
            delay_microseconds(1);
        }
        transmit_complete_flag = I2C_TRANSMISSION_IN_PROGRESS;
    }
    if (I2C_TRANSMISSION_ABORTED == transmit_complete_flag)
    {
        err = FSP_ERR_ABORTED;
    }
    return err;
}
/**********************************************************************************************************************
 End of function icp_rmcom_i2c_r
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: icp_rmcom_i2c_w
 * Description  : .
 * Arguments    : reg register address, buffer pointer to store read data and number of bytes to read
 *              : val
 *              : num
 * Return Value : FSP_SUCCESS         Upon successful open and start of timer
 *                Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 *********************************************************************************************************************/
fsp_err_t icp_rmcom_i2c_w(uint8_t reg, uint8_t *val, uint8_t num)
{
    uint16_t timeout = 1000;
    fsp_err_t err;
    static uint8_t s_data[50];
    s_data[0] = reg;
    memcpy(&(s_data[1]), val, (size_t)num);
    transmit_complete_flag = I2C_TRANSMISSION_IN_PROGRESS;

    err = g_comms_i2c_device4.p_api->write(g_comms_i2c_device4.p_ctrl, s_data, (uint32_t)(num + 1));
    if (FSP_SUCCESS == err)
    {
        while (I2C_TRANSMISSION_IN_PROGRESS == transmit_complete_flag)
        {
            if (0 == (--timeout))
            {
                break;
            }
            delay_microseconds(1);
        }
        transmit_complete_flag = I2C_TRANSMISSION_IN_PROGRESS;
    }

    if (I2C_TRANSMISSION_ABORTED == transmit_complete_flag)
    {
        err = FSP_ERR_ABORTED;
    }
    return err;
}
/**********************************************************************************************************************
 End of function icp_rmcom_i2c_w
 *********************************************************************************************************************/




/**********************************************************************************************************************
 * Function Name: icp_comms_i2c_callback
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void icp_comms_i2c_callback(rm_comms_callback_args_t *p_args)
{
    if (RM_COMMS_EVENT_OPERATION_COMPLETE == p_args->event)
    {
        ICP_transmit_complete_flag = I2C_TRANSMISSION_COMPLETE;
    }
    else
    {
        ICP_transmit_complete_flag = I2C_TRANSMISSION_ABORTED;
    }
}
/**********************************************************************************************************************
 End of function icp_comms_i2c_callback
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: delay_microseconds
 * Description  : delay specified number of microseconds
 * Argument     : us
 * Return Value : .
 *********************************************************************************************************************/
void delay_microseconds(uint32_t us)
{
    R_BSP_SoftwareDelay(us, BSP_DELAY_UNITS_MICROSECONDS);
}
/**********************************************************************************************************************
 End of function delay_microseconds
 *********************************************************************************************************************/


/**************************************************************************************
 * Name:       ms_delay
 * Function:   delay specified number of Milliseconds
 * Parameters: ms (uint32_t) number of Milliseconds to delay
 * Return:     None
 **************************************************************************************/
void ms_delay(uint32_t ms)
{
    R_BSP_SoftwareDelay(ms, BSP_DELAY_UNITS_MILLISECONDS);
}
/**********************************************************************************************************************
 End of function ms_delay
 *********************************************************************************************************************/

