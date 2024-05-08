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
 * File Name    : ra_hs3001.c
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

/*
 * ra_hs3001.c
 *
 *  Created on: 16-Dec-2021
 *      Author: 26879
 */
#include <ra_hs3001.h>
#include "common_utils.h"
#include "sensor_thread.h"
#include "user_choice.h"
/* Utility MACRO to loop 50ms delays until expression becomes false*/
#define WAIT_WHILE_FALSE(e) while(!e) { __asm(" nop"); }

/* Variable declarations*/
static volatile fsp_err_t   g_err;    /* FSP Error variable*/
volatile sensor_demo_data_t g_demo_data; /* Demo data struct*/


/**********************************************************************************************************************
 * Function Name: g_hs300x_sensor0_quick_setup
 * Description  : Quick setup for g_hs300x_sensor0.
 * Return Value : .
 *********************************************************************************************************************/
void g_hs300x_sensor0_quick_setup(void)
{
    fsp_err_t err;

    /* Open HS300X sensor instance, this must be done before calling any HS300X API */
    err = g_hs300x_sensor0.p_api->open(g_hs300x_sensor0.p_ctrl, g_hs300x_sensor0.p_cfg);
    APP_PRINT("HS3001 sensor setup success:%d\n",err);

}
/**********************************************************************************************************************
 End of function g_hs300x_sensor0_quick_setup
 *********************************************************************************************************************/


volatile bool g_hs300x_completed = false;


/**********************************************************************************************************************
 * Function Name: hs3001_get
 * Description  : Function used to read the hs3001 sensor
 * Return Value : .
 *********************************************************************************************************************/
void hs3001_get(void)
{
    rm_hs300x_raw_data_t    th_rawdata;
    rm_hs300x_data_t        hs300x_data;

    /* start measurement */
    g_hs300x_completed = false;
    delay(50);
    g_err = g_hs300x_sensor0.p_api->measurementStart(g_hs300x_sensor0.p_ctrl);
    assert (FSP_SUCCESS == g_err);
    WAIT_WHILE_FALSE (g_hs300x_completed);

    /* Keep attempting to read the data until it stabilises.
     * Section 6.6 of the HS3001 datasheet stipulates a range of
     * measurement times with corresponding to the range of
     * sensor resolutions. */
    do
    {
        /* read the data */
        g_hs300x_completed = false;
        delay(50);
        g_err = g_hs300x_sensor0.p_api->read(g_hs300x_sensor0.p_ctrl, &th_rawdata);
        assert (FSP_SUCCESS == g_err);

        WAIT_WHILE_FALSE (g_hs300x_completed);

        /* Calculate results */
        g_err = g_hs300x_sensor0.p_api->dataCalculate(g_hs300x_sensor0.p_ctrl, &th_rawdata, &hs300x_data);
        assert((FSP_SUCCESS == g_err) || (g_err == FSP_ERR_SENSOR_INVALID_DATA));
    }
    while (FSP_ERR_SENSOR_INVALID_DATA == g_err);

    g_demo_data.gs_demo_humidity     = (float32_t)hs300x_data.humidity.integer_part +
                                        ((float32_t)hs300x_data.humidity.decimal_part * 0.01F);
    g_demo_data.gs_demo_temperature  = (float32_t)hs300x_data.temperature.integer_part +
                                        ((float32_t)hs300x_data.temperature.decimal_part * 0.01F);

}
/**********************************************************************************************************************
 End of function hs3001_get
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: hs300x_callback
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void hs300x_callback(rm_hs300x_callback_args_t * p_args)
{
    if (RM_HS300X_EVENT_SUCCESS == p_args->event)
    {
        g_hs300x_completed = true;
    }
    else if (RM_HS300X_EVENT_ERROR == p_args->event)
    {
        APP_PRINT("\n------RM_HS300X_EVENT_ERROR-------\n");
    }
    else
    {
        APP_PRINT("\n/////////hs300x_callback/////////\n");
    }
    FSP_PARAMETER_NOT_USED(p_args);

}
/**********************************************************************************************************************
 End of function hs300x_callback
 *********************************************************************************************************************/


