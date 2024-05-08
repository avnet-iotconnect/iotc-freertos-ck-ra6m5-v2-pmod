/***********************************************************************************************************************
 * File Name    : user_choice.c
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
#include "user_choice.h"
#include "common_utils.h"
#include "common_init.h"
#include "common_utils.h"
#include "icm42605.h"

/* Demo data structures*/
extern volatile         sensor_demo_data_t g_demo_data;
extern volatile         rm_zmod4xxx_iaq_1st_data_t g_iaq_data;
extern volatile float32_t   gs_demo_air_quality;

extern char_t g_corracc_x[20];
extern char_t g_corracc_y[20];
extern char_t g_corracc_z[20];
extern char_t g_gval_x[20];
extern char_t g_gval_y[20];
extern char_t g_gval_z[20];
extern char_t g_corgyro_x[20];
extern char_t g_corgyro_y[20];
extern char_t g_corgyro_z[20];

extern xyzfloat g_corraccraw;
extern xyzfloat g_accraw;
extern xyzfloat g_gval;
extern xyzfloat g_gyrraw;
extern xyzfloat g_gyr;
extern xyzfloat g_magvalue;

xyzfloat g_prev_corraccraw;
xyzfloat g_prev_accraw;
xyzfloat g_prev_gval;
xyzfloat g_prev_gyrraw;
xyzfloat g_prev_gyr;
xyzfloat g_prev_magvalue;

extern float32_t resultantG;

extern float32_t Temperature;
extern float32_t Pressure;

float32_t        g_prev_temperature;
float32_t        g_prev_pressure;

static volatile iaq_demo_data_t     prev_IAQ_data;
static volatile sensor_demo_data_t  g_previous_data;
static volatile float32_t               prev_gs_oaq_air_quality;

char_t flt_oaq_str1[20]   = {RESET_VALUE};
char_t flt_etoh_str1[20]  = {RESET_VALUE};
char_t flt_eco2_str1[20]  = {RESET_VALUE};
char_t flt_tvoc_str1[20]  = {RESET_VALUE};
char_t flt_humi_str1[20]  = {RESET_VALUE};
char_t flt_temp_str1[20]  = {RESET_VALUE};
char_t Tempficp[20]       = {RESET_VALUE};
char_t PressF[20]         = {RESET_VALUE};

volatile uint16_t   user_time         = 0;
volatile bool_t     data_update         = false;
volatile bool_t     data_update_42605   = false;
volatile bool_t     data_update_4410    = false;
volatile bool_t     data_update_4510    = false;


extern const char_t * const gp_clear_screen;
extern const char_t * const gp_cursor_home;



/*******************************************************************************************************************//**
 * @brief       Timer call back after a interval of time set by user
 * @param[in]   p_args
 * @retval
 * @retval
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: g_user_timer_cb
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void g_user_timer_cb(timer_callback_args_t *p_args)
{
    (void)(p_args);
    user_time++;
    if (user_time>USER_TIMER_DATA_POST)
    {
        compare_current_data_with_previous_sample();
    }
}
/**********************************************************************************************************************
 End of function g_user_timer_cb
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: user_timer_start
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void user_timer_start(void)
{
    g_timer1.p_api->open(g_timer1.p_ctrl, g_timer1.p_cfg);
    g_timer1.p_api->enable(g_timer1.p_ctrl);
    g_timer1.p_api->start(g_timer1.p_ctrl);
}
/**********************************************************************************************************************
 End of function user_timer_start
 *********************************************************************************************************************/


/*******************************************************************************************************************//**
 * @brief       Reset user timer
 * @param[in]
 * @retval
 * @retval
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: user_timer_reset
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void user_timer_reset(void)
{
    g_timer1.p_api->stop(g_timer1.p_ctrl);
    g_timer1.p_api->disable(g_timer1.p_ctrl);
    g_timer1.p_api->reset(g_timer1.p_ctrl);
    g_timer1.p_api->close(g_timer1.p_ctrl);
    user_time = 0;
    user_timer_start();
}
/**********************************************************************************************************************
 End of function user_timer_reset
 *********************************************************************************************************************/


/*******************************************************************************************************************//**
 * @brief       Compare current sensor data with previous data
 * @param[in]
 * @retval
 * @retval
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: compare_current_data_with_previous_sample
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void compare_current_data_with_previous_sample(void)
{
    APP_PRINT(gp_clear_screen);
    APP_PRINT(gp_cursor_home);

    APP_PRINT("**********************************************\r\n");
#if HS3001_SENSOR_ENABLE
    if ((!(compare_float(g_previous_data.gs_demo_humidity, g_demo_data.gs_demo_humidity))) || \
        (!(compare_float(g_previous_data.gs_demo_temperature, g_demo_data.gs_demo_temperature))))
    {
        data_update = true;

        g_previous_data.gs_demo_humidity    = g_demo_data.gs_demo_humidity;
        g_previous_data.gs_demo_temperature = g_demo_data.gs_demo_temperature;
        print_float(flt_humi_str1, sizeof(flt_humi_str1), g_previous_data.gs_demo_humidity);
        print_float(flt_temp_str1, sizeof(flt_temp_str1), g_demo_data.gs_demo_temperature);
        APP_PRINT("\n HS3001 TEMPERATURE DATA \r\n= %s \r\n", flt_temp_str1);
        APP_PRINT("\n HS3001 HUMIDITY DATA \r\n = %s \r\n",flt_humi_str1);
    }
#endif
#if ZMOD4410_SENSOR_ENABLE
    if ((!(compare_float(prev_IAQ_data.gs_demo_eco2, g_iaq_data.eco2))) || \
            (!(compare_float(prev_IAQ_data.gs_demo_etoh, !g_iaq_data.etoh))) || \
            (!(compare_float(prev_IAQ_data.gs_demo_tvoc, g_iaq_data.tvoc))))
    {
        data_update = true;
        data_update_4410 = true;

        prev_IAQ_data.gs_demo_eco2 = g_iaq_data.eco2;
        prev_IAQ_data.gs_demo_etoh = g_iaq_data.etoh;
        prev_IAQ_data.gs_demo_tvoc = g_iaq_data.tvoc;
        prev_IAQ_data.gs_demo_iaq  = g_iaq_data.iaq;

        print_float(flt_eco2_str1, sizeof(flt_eco2_str1), g_iaq_data.eco2);
        print_float(flt_etoh_str1, sizeof(flt_etoh_str1), g_iaq_data.etoh);
        print_float(flt_tvoc_str1, sizeof(flt_tvoc_str1), g_iaq_data.tvoc);
        APP_PRINT("\nZMOD4410 ECO2 DATA \r ECO= %s \r\n",flt_eco2_str1);
        APP_PRINT("\nZMOD4410 ETOH DATA \r ETOH = %s \r\n",flt_etoh_str1);
        APP_PRINT("\nZMOD4410 TVOC DATA \r TVOC= %s \r\n",flt_tvoc_str1);
    }
#endif


#if ICM42605_SENSOR_ENABLE
    if ((!(compare_float(g_prev_corraccraw.x, g_corraccraw.x))) || \
            (!(compare_float(g_prev_corraccraw.y, g_corraccraw.y))) || \
            (!(compare_float(g_prev_corraccraw.z, g_corraccraw.z))))
    {
        data_update         = true;
        data_update_42605   = true;

        APP_PRINT("\nICM42605 Data\r\n");
        g_prev_corraccraw = g_corraccraw;

        APP_PRINT("Accelerometer corrected Raw Values\r\n");
        print_float(g_corracc_x, sizeof(g_corracc_x), g_corraccraw.x);
        print_float(g_corracc_y, sizeof(g_corracc_y), g_corraccraw.y);
        print_float(g_corracc_z, sizeof(g_corracc_z), g_corraccraw.z);
        APP_PRINT("X %s Y %s Z %s\r\n", g_corracc_x, g_corracc_y, g_corracc_z);
    }

    if ((!(compare_float (g_prev_gval.x, g_gval.x))) || \
            (!(compare_float (g_prev_gval.y, g_gval.y))) || \
            (!(compare_float (g_prev_gval.z, g_gval.z))))
    {
        char_t resultg[20];

        data_update = true;

        g_prev_gval = g_gval;
        APP_PRINT("\nG Values\r");
        print_float(g_gval_x, sizeof(g_gval_x), g_gval.x);
        print_float(g_gval_y, sizeof(g_gval_y), g_gval.y);
        print_float(g_gval_z, sizeof(g_gval_z), g_gval.z);
        APP_PRINT("%s,%s,%s\r\n", g_gval_x, g_gval_y, g_gval_z);
        APP_PRINT("\nResultant g values\r");
        print_float(resultg, sizeof(resultg), resultantG);
        APP_PRINT("%s\r\n", resultg);
    }

    if ((!(compare_float(g_prev_gyr.x, g_gyr.x))) || \
            (!(compare_float(g_prev_gyr.y, g_gyr.y))) || \
            (!(compare_float(g_prev_gyr.z, g_gyr.z))))
    {
        data_update = true;

        g_prev_gyr = g_gyr;
        APP_PRINT("\nGyro corrected Raw Values\r");
        print_float(g_corgyro_x, sizeof(g_corgyro_x), g_gyr.x);
        print_float(g_corgyro_y, sizeof(g_corgyro_y), g_gyr.y);
        print_float(g_corgyro_z, sizeof(g_corgyro_z), g_gyr.z);
        APP_PRINT("%s,%s,%s\r\n", g_corgyro_x, g_corgyro_y, g_corgyro_z);
    }
#endif

#if ICP20100_SENSOR_ENABLE
    if ((!(compare_float(g_prev_temperature, Temperature))) || \
            (!(compare_float(g_prev_pressure, Pressure))))
    {
        data_update = true;

        print_float(Tempficp,sizeof(Tempficp),Temperature);
        print_float(PressF,sizeof(PressF),Pressure);
        APP_PRINT("\nICP20100 Data Temp = %s \nPressure in pascal = %s\r\n",Tempficp,PressF);
    }
#endif

#if ZMOD4510_SENSOR_ENABLE
    if (!(compare_float(prev_gs_oaq_air_quality, gs_demo_air_quality)))
    {
        data_update      = true;
        data_update_4510 = true;

        prev_gs_oaq_air_quality = gs_demo_air_quality;
        print_float(flt_oaq_str1, sizeof(flt_oaq_str1), prev_gs_oaq_air_quality);
        APP_PRINT("\nZMOD4510 OAQ DATA OAQ = %s \r\n",flt_oaq_str1);
    }
#endif
        APP_PRINT("**********************************************\r\n");
        user_timer_reset();
}
/**********************************************************************************************************************
 End of function compare_current_data_with_previous_sample
 *********************************************************************************************************************/


extern bool_t r_ob1203_get_sensing_status(st_sensorsOB_t *p_data);

/**********************************************************************************************************************
 * Function Name: sensor_thread_get_status
 * Description  : .
 * Argument     : p_data
 * Return Value : .
 *********************************************************************************************************************/
void sensor_thread_get_status(st_sensor_data_t *p_data)
{
    if (true == data_update)
    {
        float64_t t1 = 0.0;

        p_data->hs300x.hs300x_data.humidity.integer_part = (int16_t)abs((int_t)g_previous_data.gs_demo_humidity);
        t1 = g_previous_data.gs_demo_humidity  - (p_data->hs300x.hs300x_data.humidity.integer_part * 1.0);
        p_data->hs300x.hs300x_data.humidity.decimal_part = (int16_t)abs((int_t)(t1 * 100));

        p_data->hs300x.hs300x_data.temperature.integer_part = (int16_t)abs((int_t)g_previous_data.gs_demo_temperature);
        t1 = g_previous_data.gs_demo_temperature  - (p_data->hs300x.hs300x_data.temperature.integer_part * 1.0);
        p_data->hs300x.hs300x_data.temperature.decimal_part = (int16_t)abs((int_t)(t1 * 100));

        p_data->icp20100.pressureicp = Pressure;
        p_data->icp20100.temperatureicp = Temperature;

        if (true == data_update_4510)
        {
            p_data->zmod4510.calibrated = data_update_4510;
            p_data->zmod4510.zmod410_oaq = gs_demo_air_quality;
        }

        if (true == data_update_42605)
        {
            p_data->icm42605.available = true;

            p_data->icm42605.my_accel.x = g_corraccraw.x;
            p_data->icm42605.my_accel.y = g_corraccraw.y;
            p_data->icm42605.my_accel.z = g_corraccraw.z;

            p_data->icm42605.my_gyro.x  = g_gyr.x;
            p_data->icm42605.my_gyro.y  = g_gyr.y;
            p_data->icm42605.my_gyro.z  = g_gyr.z;
        }

        if (0 != p_data->hs300x.hs300x_data.humidity.integer_part)
        {
            p_data->refresh_console = true;
            data_update             = false;
        }
    }
    r_ob1203_get_sensing_status(&p_data->ob1203);
    r_zmod4410_get_sensing_status(&p_data->zmod4410);
}
/**********************************************************************************************************************
 End of function sensor_thread_get_status
 *********************************************************************************************************************/


