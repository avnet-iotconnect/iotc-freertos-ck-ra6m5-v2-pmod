/***********************************************************************************************************************
 * File Name    : ra_zmod4xxx_iaq1stgen.c
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
 * This code was created by Dan Allen, 2020, Renesas Electronics America. It is intended for demonstration purposes
 * only, not for production. No performance or suitability for any purpose including medical devices is guaranteed or
 * claimed. For questions and inquiries visit Renesas.com

 **********************************************************************************************************************/

#include "hal_data.h"
#include "ra_zmod4xxx_common.h"
#include "common_utils.h"
#include "sensor_thread.h"
#include "user_choice.h"

volatile rm_zmod4xxx_iaq_1st_data_t g_iaq_data;
static demo_sequence_t sequence = DEMO_SEQUENCE_1;
static bool_t ZMOD4410_calibrated = false;
rm_zmod4xxx_raw_data_t iaq_raw_data;

/**********************************************************************************************************************
 * Function Name: start_iaq_1st_gen_demo
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void start_iaq_1st_gen_demo(void)
{
    fsp_err_t err;

    rm_zmod4xxx_iaq_1st_data_t zmod4xxx_data;
    switch (sequence)
    {
        case DEMO_SEQUENCE_1 :
        {
            /* Clear status */
            g_i2c_callback_status = DEMO_CALLBACK_STATUS_WAIT;
#if DEMO_IRQ_ENABLE
            g_irq_callback_status = DEMO_CALLBACK_STATUS_WAIT;
#endif

            /* Start measurement */
            err = g_zmod4xxx_sensor0.p_api->measurementStart(g_zmod4xxx_sensor0.p_ctrl);
            if (FSP_SUCCESS == err)
            {
                sequence = DEMO_SEQUENCE_2;
            }
            else
            {
                APP_PRINT ("\r\nTask zmod4410 task error in seq 1:%d\r\n",err);
            }
            break;
        }


        case DEMO_SEQUENCE_2 :
        {
            /* Check I2C callback status */
            switch (g_i2c_callback_status)
            {
                case DEMO_CALLBACK_STATUS_WAIT :
                    break;
                case DEMO_CALLBACK_STATUS_SUCCESS :
                    sequence = DEMO_SEQUENCE_3;
                    break;
                case DEMO_CALLBACK_STATUS_REPEAT :
                    sequence = DEMO_SEQUENCE_1;
                    break;
                default :
                    APP_PRINT ("\r\nTask zmod4410 task delete in seq 2:%d\r\n",err);
                    break;
            }
            vTaskDelay(10);
            break;
        }


#if DEMO_IRQ_ENABLE
        case DEMO_SEQUENCE_3 :
        {
            /* Check IRQ callback status */
            switch (g_irq_callback_status)
            {
                case DEMO_CALLBACK_STATUS_WAIT :
                    break;
                case DEMO_CALLBACK_STATUS_SUCCESS :
                    sequence = DEMO_SEQUENCE_5;
                    break;
                default :
                    break;
            }
            vTaskDelay(10);
            break;
        }

#else
        case DEMO_SEQUENCE_3 :
        {
            /* Clear status */
            g_i2c_callback_status = DEMO_CALLBACK_STATUS_WAIT;

            /* Get status */
            err = g_zmod4xxx_sensor0.p_api->statusCheck(g_zmod4xxx_sensor0.p_ctrl);
            if (FSP_SUCCESS == err)
            {
                sequence = DEMO_SEQUENCE_4;
                APP_PRINT ("\r\nTask zmod4410 status Successful:%d\r\n",err);
            }
            else
            {
                APP_PRINT ("\r\nTask zmod4410 task delete in seq 3:%d\r\n",err);
            }
            break;
        }

        case DEMO_SEQUENCE_4 :
        {
            /* Check I2C callback status */
            switch (g_i2c_callback_status)
            {
                case DEMO_CALLBACK_STATUS_WAIT :
                    break;
                case DEMO_CALLBACK_STATUS_SUCCESS :
                    sequence = DEMO_SEQUENCE_5;
                    break;
                case DEMO_CALLBACK_STATUS_REPEAT :
                    sequence = DEMO_SEQUENCE_3;
                    break;
                default :
                    APP_PRINT ("\r\nTask zmod4410 task delete in seq 4:%d\r\n",err);
                    break;
            }
        break;
        }

#endif
        case DEMO_SEQUENCE_5 :
        {
            /* Clear status */
            g_i2c_callback_status = DEMO_CALLBACK_STATUS_WAIT;

            /* Read data */
            err = g_zmod4xxx_sensor0.p_api->read(g_zmod4xxx_sensor0.p_ctrl, &iaq_raw_data);
            if (FSP_SUCCESS == err)
            {
                sequence = DEMO_SEQUENCE_6;
            }
            else if (FSP_ERR_SENSOR_MEASUREMENT_NOT_FINISHED == err)
            {
                sequence = DEMO_SEQUENCE_3;
                /* Delay 5ms */
                vTaskDelay(5);
                APP_PRINT ("\r\nTask zmod4410 task read failed in seq 5:%d\r\n",err);
            }else
            {
                ;/*none*/
            }
            break;
        }


        case DEMO_SEQUENCE_6 :
        {
            /* Check I2C callback status */
            switch (g_i2c_callback_status)
            {
                case DEMO_CALLBACK_STATUS_WAIT :
                    break;
                case DEMO_CALLBACK_STATUS_SUCCESS :
                    sequence = DEMO_SEQUENCE_7;
                    break;
                case DEMO_CALLBACK_STATUS_REPEAT :
                    sequence = DEMO_SEQUENCE_5;
                    break;
                default :
                    APP_PRINT ("\r\nTask zmod4410 task delete in seq 6:%d\r\n",err);
                    break;
            }
            vTaskDelay(10);
            break;
        }


        case DEMO_SEQUENCE_7 :
        {
            /* Calculate data */
            err = g_zmod4xxx_sensor0.p_api->iaq1stGenDataCalculate
                    (g_zmod4xxx_sensor0.p_ctrl, &iaq_raw_data, &zmod4xxx_data);
            if (FSP_SUCCESS == err)
            {
#if RM_ZMOD4XXX_CFG_OPERATION_MODE == 6
                sequence = DEMO_SEQUENCE_3;
#if DEMO_IRQ_ENABLE
                g_irq_callback_status = DEMO_CALLBACK_STATUS_WAIT;
#endif
#elif RM_ZMOD4XXX_CFG_OPERATION_MODE == 2
                sequence = DEMO_SEQUENCE_1;
#endif

                /* Set data */
                g_iaq_data.etoh  = zmod4xxx_data.etoh;
                g_iaq_data.eco2  = zmod4xxx_data.eco2;
                g_iaq_data.tvoc  = zmod4xxx_data.tvoc;
                g_iaq_data.iaq   = zmod4xxx_data.iaq;

                ZMOD4410_calibrated = true;
            }
            else if (FSP_ERR_SENSOR_IN_STABILIZATION == err)
            {
#if RM_ZMOD4XXX_CFG_OPERATION_MODE == 6
                sequence = DEMO_SEQUENCE_3;
                APP_PRINT ("\r\nTask zmod4410 in stabilization:%d\r\n",err);
#if DEMO_IRQ_ENABLE
                g_irq_callback_status = DEMO_CALLBACK_STATUS_WAIT;
#endif
#elif RM_ZMOD4XXX_CFG_OPERATION_MODE == 2
                sequence = DEMO_SEQUENCE_1;
#endif
            }
            else
            {
                APP_PRINT ("\r\nTask zmod4410 task delete in seq 7:%d\r\n",err);
            }
            sequence = DEMO_SEQUENCE_3;
            vTaskDelay(5);/* Delay required time */
            break;
        }


        default :
        {
            APP_PRINT ("\r\nTask zmod4410 task delete in default case:%d\r\n",err);
            break;
        }

    }
}

/**********************************************************************************************************************
 * Function Name: r_zmod4410_get_sensing_status
 * Description  : .
 * Argument     : p_data
 * Return Value : .
 *********************************************************************************************************************/
bool_t r_zmod4410_get_sensing_status(st_sensorsZmod4410_t *p_data)
{
    p_data->zmod4410Data.ec02   =  g_iaq_data.eco2;
    p_data->zmod4410Data.etoh   =  g_iaq_data.etoh;
    p_data->zmod4410Data.tvoc   =  g_iaq_data.tvoc;
    p_data->zmod4410Data.iaq    =  g_iaq_data.iaq;

    if (true == ZMOD4410_calibrated)
    {
        ZMOD4410_calibrated = true;
    }

    p_data->calibrated = ZMOD4410_calibrated;
    return (true);
}
/**********************************************************************************************************************
 End of function r_zmod4410_get_sensing_status
 *********************************************************************************************************************/

