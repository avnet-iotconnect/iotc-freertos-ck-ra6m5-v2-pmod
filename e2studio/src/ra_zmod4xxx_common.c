/***********************************************************************************************************************
 * File Name    : ra_zmod4xxx_common.c
 * Description  : Contains data structures and function definitions of ZMOD4XXX sensor
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
 * This code was created by Dan Allen, 2020, Renesas Electronics America. It is intended for demonstration purposes
 * only not for production. No performance or suitability for any purpose including medical devices is guaranteed or
 * claimed. For questions and inquiries visit Renesas.com
 **********************************************************************************************************************/

#include "ra_hs3001.h"
#include "hal_data.h"
#include "ra_zmod4xxx_common.h"
#include "common_utils.h"
#include "sensor_thread.h"
#include "oaq_thread0.h"

volatile demo_callback_status_t  g_i2c_callback_status;
volatile demo_callback_status_t  g_irq_callback_status;
volatile demo_callback_status_t  g_i2c_callback_status1;
volatile demo_callback_status_t  g_irq_callback_status1;
volatile demo_callback_status_t  g_i2c_callback_status2;
volatile demo_callback_status_t  g_irq_callback_status2;


/**********************************************************************************************************************
 * Function Name: g_zmod4xxx_sensor0_quick_setup
 * Description  : Quick setup for g_zmod4xxx_sensor 0.
 * Return Value : .
 *********************************************************************************************************************/
void g_zmod4xxx_sensor0_quick_setup(void)
{
    fsp_err_t err;

    /* Open ZMOD4XXX sensor instance, this must be done before calling any ZMOD4XXX API */
    err = g_zmod4xxx_sensor0.p_api->open(g_zmod4xxx_sensor0.p_ctrl, g_zmod4xxx_sensor0.p_cfg);
    APP_PRINT ("\r\nTask zmod4410 sensor setup Successfully:%d\r\n",err);
}
/**********************************************************************************************************************
 End of function g_zmod4xxx_sensor0_quick_setup
 *********************************************************************************************************************/


#if 0
/* Quick setup for g_zmod4xxx_sensor 1. */
void g_zmod4xxx_sensor1_quick_setup(void)
{
    fsp_err_t err;

    /* Open ZMOD4XXX sensor instance, this must be done before calling any ZMOD4XXX API */
    err = g_zmod4xxx_sensor1.p_api->open(g_zmod4xxx_sensor1.p_ctrl, g_zmod4xxx_sensor1.p_cfg);
    APP_PRINT ("\r\nTask zmod4510 sensor setup Successfully:%d\r\n",err);
}
#endif

/**********************************************************************************************************************
 * Function Name: g_zmod4xxx_sensor2_quick_setup
 * Description  : Quick setup for g_zmod4xxx_sensor 2.
 * Return Value : .
 *********************************************************************************************************************/
void g_zmod4xxx_sensor2_quick_setup(void)
{
    fsp_err_t err;

    /* Open ZMOD4XXX sensor instance, this must be done before calling any ZMOD4XXX API */
    err = g_zmod4xxx_sensor2.p_api->open(g_zmod4xxx_sensor2.p_ctrl, g_zmod4xxx_sensor2.p_cfg);
    APP_PRINT ("\r\nTask zmod4510 sensor setup Successfully:%d\r\n", err);
}
/**********************************************************************************************************************
 End of function g_zmod4xxx_sensor2_quick_setup
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: zmod4xxx_comms_i2c0_callback
 * Description  : ZMOD4410 I2C callback
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void zmod4xxx_comms_i2c0_callback(rm_zmod4xxx_callback_args_t * p_args)
{
    if (RM_ZMOD4XXX_EVENT_SUCCESS == p_args->event)
    {
        g_i2c_callback_status = DEMO_CALLBACK_STATUS_SUCCESS;
    }
    else if (RM_ZMOD4XXX_EVENT_MEASUREMENT_COMPLETE == p_args->event)
    {
        g_i2c_callback_status = DEMO_CALLBACK_STATUS_SUCCESS;
    }
    else
    {
        g_i2c_callback_status = DEMO_CALLBACK_STATUS_REPEAT;
    }

}


/**********************************************************************************************************************
 * Function Name: zmod4xxx_comms_i2c1_callback
 * Description  : ZMOD4510 I2C callback.
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void zmod4xxx_comms_i2c1_callback(rm_zmod4xxx_callback_args_t * p_args)
{
    if (RM_ZMOD4XXX_EVENT_SUCCESS == p_args->event)
    {
        g_i2c_callback_status1 = DEMO_CALLBACK_STATUS_SUCCESS;
    }
    else if (RM_ZMOD4XXX_EVENT_MEASUREMENT_COMPLETE == p_args->event)
    {
        g_i2c_callback_status1 = DEMO_CALLBACK_STATUS_SUCCESS;
    }
    else
    {
        g_i2c_callback_status1 = DEMO_CALLBACK_STATUS_REPEAT;
    }

}
/**********************************************************************************************************************
 End of function zmod4xxx_comms_i2c1_callback
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: zmod4xxx_comms_i2c2_callback
 * Description  : ZMOD4510 I2C callback.
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void zmod4xxx_comms_i2c2_callback(rm_zmod4xxx_callback_args_t * p_args)
{
    if (RM_ZMOD4XXX_EVENT_SUCCESS == p_args->event)
    {
        g_i2c_callback_status2 = DEMO_CALLBACK_STATUS_SUCCESS;
    }
    else if (RM_ZMOD4XXX_EVENT_MEASUREMENT_COMPLETE == p_args->event)
    {
        g_i2c_callback_status2 = DEMO_CALLBACK_STATUS_SUCCESS;
    }
    else
    {
        g_i2c_callback_status2 = DEMO_CALLBACK_STATUS_REPEAT;
    }

}
/**********************************************************************************************************************
 End of function zmod4xxx_comms_i2c2_callback
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: zmod4xxx_irq0_callback
 * Description  : ZMOD4410 interrupt callback.
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void zmod4xxx_irq0_callback(rm_zmod4xxx_callback_args_t * p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    g_irq_callback_status = DEMO_CALLBACK_STATUS_SUCCESS;

}
/**********************************************************************************************************************
 End of function zmod4xxx_irq0_callback
 *********************************************************************************************************************/


/* ZMOD4510 interrupt callback */
/**********************************************************************************************************************
 * Function Name: zmod4xxx_irq15_callback
 * Description  : ZMOD4510 interrupt callback.
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void zmod4xxx_irq15_callback(rm_zmod4xxx_callback_args_t * p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    g_irq_callback_status1 = DEMO_CALLBACK_STATUS_SUCCESS;

}
/**********************************************************************************************************************
 End of function zmod4xxx_irq15_callback
 *********************************************************************************************************************/



