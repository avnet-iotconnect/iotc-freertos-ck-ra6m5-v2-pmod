/***********************************************************************************************************************
 * File Name    : i2c.c
 * Description  : Contains data structures and functions used for I2C
 ***********************************************************************************************************************/
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
 ************************************************************************************************************************
 * This code was created by Dan Allen, 2020, Renesas Electronics America. It is intended for demonstration purposes only, not
 * for production. No performance or suitability for any purpose including medical devices is guaranteed or claimed. For
 * questions and inquiries visit Renesas.com

 **********************************************************************************************************************/

/***********************************************************************************************************************
 Includes
 ***********************************************************************************************************************/
#include "bsp_api.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "i2c.h"
#include "hal_data.h"
#include "common_utils.h"
#include "oximeter.h"

/***********************************************************************************************************************
 Pragma directive
 ***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
#define I2C_TRANSMISSION_IN_PROGRESS  (0)
#define I2C_TRANSMISSION_COMPLETE     (1)
#define I2C_TRANSMISSION_ABORTED      (2)
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
 Global variables and functions
 ***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
static uint8_t transmit_complete_flag;
extern const rm_comms_instance_t g_comms_ob1203_bio_sensor;


/**********************************************************************************************************************
 * Function Name: i2c_r
 * Description  : .
 * Arguments    : addr
 *              : reg
 *              : val
 *              : num
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t i2c_r(uint8_t addr, uint8_t reg, uint8_t *val, char num)
{
    (void)(addr);
    uint16_t timeout        = 1000;
    transmit_complete_flag  = I2C_TRANSMISSION_IN_PROGRESS;

    fsp_err_t err;

    rm_comms_write_read_params_t write_read_params;

    write_read_params.p_src      =  &reg;
    write_read_params.src_bytes  =  1;
    write_read_params.p_dest     =  val;
    write_read_params.dest_bytes =  (uint8_t)num;

    err = g_comms_ob1203_bio_sensor.p_api->writeRead(g_comms_ob1203_bio_sensor.p_ctrl, write_read_params);
    if (FSP_SUCCESS == err)
    {
        while (I2C_TRANSMISSION_IN_PROGRESS == transmit_complete_flag)
        {
            if (0 == (--timeout))
            {
                break;
            }
            delayMicroseconds(1);
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
 End of function i2c_r
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: i2c_w
 * Description  : I2C write.
 * Arguments    : addr
 *              : reg
 *              : val
 *              : num
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t i2c_w(uint8_t addr, uint8_t reg, uint8_t *val, char num)
{
    (void)(addr);
    uint16_t timeout = 1000;

    int8_t err;

    static uint8_t s_data[50];
    s_data[0] = reg;
    memcpy(&(s_data[1]), val, (size_t)num);
    transmit_complete_flag = I2C_TRANSMISSION_IN_PROGRESS;

    err = g_comms_ob1203_bio_sensor.p_api->write(g_comms_ob1203_bio_sensor.p_ctrl, s_data, (uint32_t)(num + 1));

    if (FSP_SUCCESS == err)
    {
        while (I2C_TRANSMISSION_IN_PROGRESS == transmit_complete_flag)
        {
            if (0 == (--timeout))
            {
                break;
            }
            delayMicroseconds(1);
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
 End of function i2c_w
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: comms_i2c_callback
 * Description  : call back parameters.
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void comms_i2c_callback(rm_comms_callback_args_t * p_args)
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
 End of function comms_i2c_callback
 *********************************************************************************************************************/


