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
 * File Name    : ob1203_thread_entry.c
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/
#include "ob1203_thread.h"
#include "common_utils.h"
#include "oximeter.h"
#include "user_choice.h"
#include "OB1203/OB1203.h"
#include "SPO2/SPO2.h"
#include "oximstruct.h"
/* OB1203 Thread entry function */

#define UNUSED(x)  (void)(x)

extern void (*p_IntB_Event)(void);

void g_ob1203_sensor0_quick_setup ();

/**********************************************************************************************************************
 * Function Name: sensorIRQCallback
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void sensorIRQCallback(external_irq_callback_args_t *p_args)
{
    UNUSED(p_args);
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (NULL != p_IntB_Event)
    {
        p_IntB_Event();
        xSemaphoreGiveFromISR(g_ob1203_semaphore, &xHigherPriorityTaskWoken);
    }
}
/**********************************************************************************************************************
 End of function sensorIRQCallback
 *********************************************************************************************************************/




/**********************************************************************************************************************
 * Function Name: g_ob1203_sensor0_quick_setup
 * Description  : .
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
void g_ob1203_sensor0_quick_setup()
{
    fsp_err_t err = FSP_SUCCESS;
    err = g_comms_ob1203_bio_sensor.p_api->open
            (g_comms_ob1203_bio_sensor.p_ctrl, g_comms_ob1203_bio_sensor.p_cfg);
    if (FSP_SUCCESS == err)
    {
        APP_PRINT("** OB1203 Sensor open success ** %u \r\n",err);
    }
}
/**********************************************************************************************************************
 End of function g_ob1203_sensor0_quick_setup
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: ob1203_thread_entry
 * Description  : .
 * Argument     : pvParameters
 * Return Value : .
 *********************************************************************************************************************/
void ob1203_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

#if OB1203_SENSOR_ENABLE
    /* Open OB1203 */
    /* OB1203 configuration */
    g_ob1203_sensor0_quick_setup();
    main_init(&ox, &sp, &ob); //do initial configuration of OB1203, Kalman filters, algorithm, etc.
#endif

    while (1)
    {
#if OB1203_SENSOR_ENABLE
        ob1203_spo2_main(&ox, &sp, &ob);
        vTaskDelay(10);
#else
        vTaskDelay(10);
//        break;
#endif
    }
}
/**********************************************************************************************************************
 End of function ob1203_thread_entry
 *********************************************************************************************************************/




