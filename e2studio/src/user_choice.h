/***********************************************************************************************************************
 * File Name    : user_choice.h
 * Description  : Contains data structures, macros and functions definitions
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

#ifndef USER_CHOICE_H_
#define USER_CHOICE_H_

#include <stdint.h>
#include "sensor_thread.h"
#include "icm42605.h"
#include "ra_zmod4xxx_common.h"

#define OB1203_SENSOR_ENABLE      (1)
#define HS3001_SENSOR_ENABLE      (1)
#define ZMOD4410_SENSOR_ENABLE    (1)
#define ZMOD4510_SENSOR_ENABLE    (1)
#define ICP20100_SENSOR_ENABLE    (1)
#define ICM42605_SENSOR_ENABLE    (1)

/* Utility struct for sensor data*/
typedef struct
{
    float32_t   gs_demo_humidity;       /* Global variable to hold humidty reading*/
    float32_t   gs_demo_temperature;    /* Global variable to hold temperature reading*/
}sensor_demo_data_t;

typedef struct
{
    float32_t gs_demo_etoh;
    float32_t gs_demo_eco2;
    float32_t gs_demo_tvoc;
    float32_t gs_demo_iaq;
}iaq_demo_data_t;


/*MACRO BASED DATA POSTING ON TIME BASIS
 Here, USER_TIMER_DATA_POST macro can be set by user based on which the data can be
 posted on interval of seconds. For e.g. if User sets it to 1 then the sensor data will
 be posted at an interval of every 1 sec*/

/* User timer delay in seconds */
#define USER_TIMER_DATA_POST        (1)

/* Function declaration */
/* Function Name: g_user_timer_cb */
extern void sensor_thread_get_status (st_sensor_data_t *p_data);
extern void g_user_timer_cb (timer_callback_args_t *p_args);
extern void user_timer_start (void);
extern void user_timer_reset (void);
extern void compare_current_data_with_previous_sample (void);

#endif /* USER_CHOICE_H_ */
