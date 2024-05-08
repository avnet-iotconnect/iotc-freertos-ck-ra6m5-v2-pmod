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
 * File Name    : menu_ns.c
 * Version      : .
 * Description  : The next steps screen display.
 *********************************************************************************************************************/

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"

#include "hal_data.h"
#include "common_init.h"
#include "common_utils.h"
#include "menu_sensor.h"
#include "console.h"

#include "rm_hs300x.h"


#define CONNECTION_ABORT_CRTL    (0x00)
#define MENU_EXIT_CRTL           (0x20)

#define MODULE_NAME      "\r\n%d. SENSOR DATA  \r\n"
#define SUB_OPTIONS1     "\r\n %c) Renesas : HS3001                  :  Waiting "
#define SUB_OPTIONS3     "\r\n %c) Renesas : OB1203SD-C4R            :  Waiting "
#define SUB_OPTIONS4     "\r\n %c) Renesas : ZMOD4510AI1R            :  Waiting "
#define SUB_OPTIONS5     "\r\n %c) Renesas : ZMOD4410AI1V            :  Waiting "
#define SUB_OPTIONS6     "\r\n %c) TDK     : ICM-42605               :  Waiting "
#define SUB_DETAILS_ICM1 "\r\n      Gyroscope     X, Y, Z deg/s      :  "
#define SUB_DETAILS_ICM2 "\r\n      Accelerometer X, Y, Z G          :  "
#define SUB_OPTIONS7     "\r\n %c) TDK     : ICP-20100               :  Waiting "
#define SUB_DETAILS_ICP1 "\r\n    Barometric pressure               :  "
#define SUB_DETAILS_ICP2 "\r\n    Sensor temperature                :  "

#define SUB_OPTIONS1D     "\r\n %c) Renesas : HS3001                                             "
#define SUB_OPTIONS1T     "\r\n    Sensor temperature                : [GREEN]%3d.%d[WHITE]degC   "
#define SUB_OPTIONS1H     "\r\n    Relative Humidity                 :  [GREEN]%2d.%d[WHITE]%%          "

#define SUB_OPTIONS6U      "\r\n %c) TDK     : ICM-42605               :  [RED]UNAVAILABLE[WHITE]       "
#define SUB_OPTIONS6D      "\r\n %c) TDK     : ICM-42605                                                        "
#define SUB_DETAILS_ICM1D  "\r\n    Gyroscope     X, Y, Z deg/s       : [GREEN]% 03.6f[WHITE], [GREEN]% 03.6f[WHITE], [GREEN]% 03.6f[WHITE]            "
#define SUB_DETAILS_ICM2D  "\r\n    Accelerometer X, Y, Z G           : [GREEN]% 03.6f[WHITE], [GREEN]% 03.6f[WHITE], [GREEN]% 03.6f[WHITE]            "
#define SUB_DETAILS_ICM1DS "\r\n    Gyroscope     X, Y, Z deg/s       : [GREEN]%s[WHITE], [GREEN]%s[WHITE], [GREEN]%s[WHITE]            "
#define SUB_DETAILS_ICM2DS "\r\n    Accelerometer X, Y, Z G           : [GREEN]%s[WHITE], [GREEN]%s[WHITE], [GREEN]%s[WHITE]            "


#define SUB_OPTIONS3I     "\r\n %c) Renesas : OB1203SD-C4R            :  [ORANGE]Idle[WHITE]                            "
#define SUB_OPTIONS3S     "\r\n %c) Renesas : OB1203SD-C4R            :  [ORANGE]Sensing[WHITE] upto 60 sec       "
#define SUB_OPTIONS3D     "\r\n %c) Renesas : OB1203SD-C4R            :  SPo2 [GREEN]%03d[WHITE]%%, HR [GREEN]%03d[WHITE]bpm, RR [GREEN]%03d[WHITE]/min  "
#define SUB_OPTIONS3DS    "\r\n %c) Renesas : OB1203SD-C4R            :  SPo2 [GREEN]%s[WHITE]%%, HR [GREEN]%s[WHITE]bpm, RR [GREEN]%s[WHITE]/min  "

#define SUB_OPTIONS4C     "\r\n %c) Renesas : ZMOD4510AI1R            :  [ORANGE]CALIBRATING[WHITE]           "
#define SUB_OPTIONS4D     "\r\n %c) Renesas : ZMOD4510AI1R                                              "
#define SUB_OPTIONS4D1    "\r\n    Outdoor Air Quality               : [GREEN]% 03.6f[WHITE]   "
#define SUB_OPTIONS4D1S   "\r\n    Outdoor Air Quality               : [GREEN]%s[WHITE]   "

#define SUB_OPTIONS5C     "\r\n %c) Renesas : ZMOD4410AI1V            :  [ORANGE]CALIBRATING[WHITE]           "
#define SUB_OPTIONS5D     "\r\n %c) Renesas : ZMOD4410AI1V                                                    "
#define SUB_OPTIONS5D1    "\r\n    Indoor Air Quality index          : [GREEN]% 03.6f[WHITE]                           "
#define SUB_OPTIONS5D2    "\r\n    Estimated Carbon Dioxide Level    : [GREEN]% 03.6f[WHITE]ppm                        "
#define SUB_OPTIONS5D3    "\r\n    Total Volatile Organic Compounds  : [GREEN]% 03.6f[WHITE]mg/m^3                     "
#define SUB_OPTIONS5DS1   "\r\n    Indoor Air Quality index          : [GREEN]%s[WHITE]                           "
#define SUB_OPTIONS5DS2   "\r\n    Estimated Carbon Dioxide Level    : [GREEN]%s[WHITE]ppm                        "
#define SUB_OPTIONS5DS3   "\r\n    Total Volatile Organic Compounds  : [GREEN]%s[WHITE]mg/m^3                     "


#define SUB_OPTIONS7D      "\r\n %c) TDK     : ICP-20100                                               "
#define SUB_DETAILS_ICP1D  "\r\n    Barometric pressure               : [GREEN]% 03.6f[WHITE]hPa  "
#define SUB_DETAILS_ICP2D  "\r\n    Sensor temperature                : [GREEN]% 03.6f[WHITE]degC "
#define SUB_DETAILS_ICP1DS "\r\n    Barometric pressure               : [GREEN]%s[WHITE]hPa  "
#define SUB_DETAILS_ICP2DS "\r\n    Sensor temperature                : [GREEN]%s[WHITE]degC "

#define SENSOR_READ_FREQUENCY_RESET_VALUE (2000)
#define ONE_BYTE                          (0x01)

static uint32_t s_count             = SENSOR_READ_FREQUENCY_RESET_VALUE;
static uint32_t s_sensor_access_ref = 0;

st_sensor_data_t s_sensor_status;

static void process_sensor_data (void);

/**********************************************************************************************************************
 * Function Name: print_float_precision
 * Description  : .
 * Arguments    : buffer
 *              : buflen
 *              : value
 * Return Value : .
 *********************************************************************************************************************/
static void print_float_precision (char_t *buffer, size_t buflen, double value)
{
    int_t integer     = (int_t) value;
    int_t fraction    = (int_t) ((value-integer) * 100.0);

    if (NULL == buffer)
    {
        return;
    }

    if (value < 0.0)
    {
        snprintf(buffer, buflen, "-%d.%02d", abs(integer), abs(fraction));
    }
    else
    {
        snprintf(buffer, buflen, " %d.%02d", abs(integer), abs(fraction));
    }
}
/**********************************************************************************************************************
 End of function print_float_precision
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: sensor_display_menu
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
test_fn sensor_display_menu(void)
{
    int8_t c = -1;

    char_t ndx = 'a';
    printf_colour ("%s%s", gp_clear_screen, gp_cursor_home);
    printf_colour("\e[?25l");
    printf_colour(MODULE_NAME, g_selected_menu);
    printf_colour(SUB_OPTIONS1, ndx++);
    printf_colour(SUB_OPTIONS3, ndx++);
    printf_colour(SUB_OPTIONS4, ndx++);
    printf_colour(SUB_OPTIONS5, ndx++);

    if (true == s_sensor_status.icm20948.available)
    {
        printf_colour(SUB_OPTIONS6, ndx++);
        printf_colour(SUB_DETAILS_ICM1);
        printf_colour(SUB_DETAILS_ICM2);
    }
    else
    {
        printf_colour(SUB_OPTIONS6, ndx++);
    }

    printf_colour(SUB_OPTIONS7, ndx++);
    printf_colour(SUB_DETAILS_ICP1);
    printf_colour(SUB_DETAILS_ICP2);

    printf_colour(MENU_RETURN_INFO2);

    while ((CONNECTION_ABORT_CRTL != c))
    {
        start_key_check();

        while (!key_pressed())
        {
            process_sensor_data();
            if (true == s_sensor_status.refresh_console)
            {
                s_sensor_status.refresh_console = false;
                ndx                             = 'a';
                printf_colour ("%s", gp_cursor_home);
                printf_colour(MODULE_NAME, g_selected_menu);
                printf_colour(SUB_OPTIONS1D, ndx++);
                printf_colour(SUB_OPTIONS1T, s_sensor_status.hs300x.hs300x_data.temperature.integer_part, s_sensor_status.hs300x.hs300x_data.temperature.decimal_part);
                printf_colour(SUB_OPTIONS1H, s_sensor_status.hs300x.hs300x_data.humidity.integer_part, s_sensor_status.hs300x.hs300x_data.humidity.decimal_part);

                if (true == s_sensor_status.ob1203.sensing)
                {
                    printf_colour(SUB_OPTIONS3S, ndx++);
                }
                else if (false == s_sensor_status.ob1203.calibrated)
                {
                    printf_colour(SUB_OPTIONS3I, ndx++);
                }
                else
                {
                    printf_colour(SUB_OPTIONS3D, ndx++, s_sensor_status.ob1203.ob_spo2, s_sensor_status.ob1203.ob_hr, s_sensor_status.ob1203.ob_rr);
                }

                if (false == s_sensor_status.zmod4510.calibrated)
                {
                    printf_colour(SUB_OPTIONS4C, ndx++);
                }
                else
                {
                    char_t tmp[32] = {RESET_VALUE};
                    printf_colour(SUB_OPTIONS4D, ndx++);
                    print_float_precision (tmp,sizeof(tmp),s_sensor_status.zmod4510.zmod410_oaq);
                    printf_colour(SUB_OPTIONS4D1S, tmp);
                }

                if (false == s_sensor_status.zmod4410.calibrated)
                {
                    printf_colour(SUB_OPTIONS5C, ndx++);
                }
                else
                {
                    char_t tmp[32]={RESET_VALUE};

                    printf_colour (SUB_OPTIONS5D,  ndx++);
                    print_float_precision (tmp, sizeof(tmp), s_sensor_status.zmod4410.zmod4410Data.iaq);
                    printf_colour (SUB_OPTIONS5DS1, tmp);
                    print_float_precision (tmp, sizeof(tmp), s_sensor_status.zmod4410.zmod4410Data.ec02);
                    printf_colour (SUB_OPTIONS5DS2, tmp);
                    print_float_precision (tmp, sizeof(tmp), s_sensor_status.zmod4410.zmod4410Data.tvoc);
                    printf_colour (SUB_OPTIONS5DS3, tmp);
                }

                if (true == s_sensor_status.icm42605.available)
                {
                    char_t tmp1[32]={RESET_VALUE};
                    char_t tmp2[32]={RESET_VALUE};
                    char_t tmp3[32]={RESET_VALUE};

                    printf_colour(SUB_OPTIONS6D, ndx++);

                    print_float_precision (tmp1, sizeof(tmp1), s_sensor_status.icm42605.my_gyro.x);
                    print_float_precision (tmp2, sizeof(tmp2), s_sensor_status.icm42605.my_gyro.y);
                    print_float_precision (tmp3, sizeof(tmp3), s_sensor_status.icm42605.my_gyro.z);
                    printf_colour(SUB_DETAILS_ICM1DS, tmp1, tmp2, tmp3);

                    print_float_precision (tmp1, sizeof(tmp1), s_sensor_status.icm42605.my_accel.x);
                    print_float_precision (tmp2, sizeof(tmp2), s_sensor_status.icm42605.my_accel.y);
                    print_float_precision (tmp3, sizeof(tmp3), s_sensor_status.icm42605.my_accel.z);
                    printf_colour(SUB_DETAILS_ICM2DS, tmp1, tmp2, tmp3);

                }
                else
                {
                    printf_colour(SUB_OPTIONS6U, ndx++);
                }

                {
                    char_t Tempficp[32]   =   {RESET_VALUE};
                    char_t PressF[32]     =   {RESET_VALUE};

                    printf_colour (SUB_OPTIONS7D, ndx++);
                    print_float_precision (PressF, sizeof(PressF), s_sensor_status.icp20100.pressureicp);
                    printf_colour (SUB_DETAILS_ICP1DS, PressF);

                    print_float_precision (Tempficp, sizeof(Tempficp), s_sensor_status.icp20100.temperatureicp);
                    printf_colour (SUB_DETAILS_ICP2DS, Tempficp);

                }
                printf_colour (MENU_RETURN_INFO);
                vTaskDelay(20);
            }
        }


        c = (int8_t)get_detected_key();

        if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
        {
            break;
        }
    }
    return (0);
}
/**********************************************************************************************************************
 End of function sensor_display_menu
 *********************************************************************************************************************/



extern void sensor_thread_get_status (st_sensor_data_t *p_data);

/**********************************************************************************************************************
 * Function Name: process_sensor_data
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void process_sensor_data(void)
{
        /* get_data_from_sensors */
        sensor_thread_get_status(&s_sensor_status);

        vTaskDelay(1);

}
/**********************************************************************************************************************
 End of function process_sensor_data
 *********************************************************************************************************************/


