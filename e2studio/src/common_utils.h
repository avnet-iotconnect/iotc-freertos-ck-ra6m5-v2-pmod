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
 * File Name    : common_utils.h
 * Description  : Contains macros, data structures and functions used  common to the EP
 *********************************************************************************************************************/
#ifndef COMMON_UTILS_H_
#define COMMON_UTILS_H_


/* generic headers */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "bsp_api.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "hal_data.h"
#include "r_typedefs.h"

#define RESET_VALUE             (0x00)

#define BUFFER_LINE_LENGTH (1024)

/* generic headers */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hal_data.h"
#include "sensor_thread.h"

/*
 * Use the define below to enable debug message through console
 * that show which files from the embedded file system are loaded
 */



/* SEGGER RTT and error related headers */
#include "SEGGER_RTT/SEGGER_RTT.h"
/**********************************************************************************************************************
 * Function Name: _write
 * Description  : .
 * Arguments    : iFile
 *              : cptr
 *              : iLen
 * Return Value : .
 *********************************************************************************************************************/
int_t write (int iFile, char *cptr, int iLen);
/**********************************************************************************************************************
 * Function Name: uart_initialize
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t uart_initialize (void);
fsp_err_t uart_print_user_msg (uint8_t *p_msg);
void delayMicroseconds (uint32_t us);
void delay (uint32_t ms);
void Delay (unsigned int xms);

#define MILLISECONDS_TO_TICKS(x) ((x) / (1000 / configTICK_RATE_HZ))

#define CARRIAGE_ASCII            (13u)     /* Carriage return */
#define ZERO_ASCII                (48u)     /* ASCII value of zero */
#define NINE_ASCII                (57u)     /* ASCII value for nine */
#define DATA_LENGTH               (20u)      /* Expected Input Data length */

#define UART_ERROR_EVENTS         (UART_EVENT_BREAK_DETECT | UART_EVENT_ERR_OVERFLOW | UART_EVENT_ERR_FRAMING | \
        UART_EVENT_ERR_PARITY)    /* UART Error event bits mapped in registers */

#define LOG_ERROR                 (1)
#define LOG_WARN                  (2)
#define LOG_INFO                  (3)
#define LOG_DEBUG                 (4)

#define Marco_Zero                (uint8_t) 0
#define Marco_One                 (uint8_t) 1
#define MACRO_TWO                 (uint8_t) 2
#define MACRO_TEN                 (uint8_t) 10

#ifndef USR_LOG_LVL
#define LOG_LVL      (LOG_ERROR)       /* error conditions   */
#else
#undef  LOG_LVL
#define LOG_LVL      (USR_LOG_LVL)     /* error conditions   */
#endif

#define RTT_TERMINAL  (1)
#define UART_TERMINAL (2)


#ifndef USR_LOG_TERMINAL
#define LOG_TERMINAL      (RTT_TERMINAL)       /* error conditions   */
#else
#define LOG_TERMINAL      (USR_LOG_TERMINAL)     /* error conditions   */
#endif
#define BIT_SHIFT_8  (8u)
#define SIZE_64      (64u)
#define SIZE_128     (128u)

#define LVL_ERR      (1u)       /* error conditions   */

#define RESET_VALUE             (0x00)
#define SEGGER_INDEX            (0)

#define APP_PRINT(fn_, ...)         if(LOG_TERMINAL == RTT_TERMINAL)\
        SEGGER_RTT_printf (SEGGER_INDEX,(fn_), ##__VA_ARGS__);\
        else \
        printf((fn_), ##__VA_ARGS__);

#define APP_ERR_PRINT(fn_, ...)     if(LOG_LVL >= LOG_ERROR){\
        if(LOG_TERMINAL == RTT_TERMINAL){\
            SEGGER_RTT_printf (SEGGER_INDEX, "[ERR] In Function: %s(), ", __FUNCTION__);\
            SEGGER_RTT_printf (SEGGER_INDEX, (fn_), ##__VA_ARGS__);\
        }\
        else {\
            printf("[ERR] In Function: %s(), ",__FUNCTION__); \
            printf(fn_, ##__VA_ARGS__);\
        }\
}

#define APP_WARN_PRINT(fn_, ...) if(LOG_LVL >= LOG_WARN){\
        if(LOG_TERMINAL == RTT_TERMINAL){\
            SEGGER_RTT_printf (SEGGER_INDEX, "[WARN] In Function: %s(), ", __FUNCTION__);\
            SEGGER_RTT_printf (SEGGER_INDEX, (fn_), ##__VA_ARGS__);\
        }\
        else {\
            printf("[WARN] In Function: %s(), ",__FUNCTION__); \
            printf(fn_, ##__VA_ARGS__);\
        }\
}

#define APP_INFO_PRINT(fn_, ...) if(LOG_LVL >= LOG_INFO){\
        if(LOG_TERMINAL == RTT_TERMINAL){\
            SEGGER_RTT_printf (SEGGER_INDEX, "[INFO] In Function: %s(), ", __FUNCTION__);\
            SEGGER_RTT_printf (SEGGER_INDEX, (fn_), ##__VA_ARGS__);\
        }\
        else {\
            printf("[INFO] In Function: %s(), ",__FUNCTION__); \
            printf((fn_), ##__VA_ARGS__);\
        }\
}

#define APP_DBG_PRINT(fn_, ...)  if(LOG_LVL >= LOG_DEBUG){\
        if(LOG_TERMINAL == RTT_TERMINAL){\
            SEGGER_RTT_printf (SEGGER_INDEX, "[DBG] In Function: %s(), ", __FUNCTION__);\
            SEGGER_RTT_printf (SEGGER_INDEX, (fn_), ##__VA_ARGS__);\
        }\
        else {\
            printf("[DBG] In Function: %s(), ",__FUNCTION__); \
            printf((fn_), ##__VA_ARGS__);\
        }\
}

#define APP_ERR_TRAP(err)        if(err){\
        if(LOG_LVL >= RTT_TERMINAL){\
            SEGGER_RTT_printf(SEGGER_INDEX, "\r\nReturned Error Code: 0x%x  \r\n", (unsigned int)err);\
            __BKPT(0);\
        }\
        else {\
            printf("\r\nReturned Error Code: 0x%x  \r\n", (unsigned int)err);\
            __BKPT(0);\
        }\
}

#define APP_READ(read_data)     (SEGGER_RTT_Read (SEGGER_INDEX, read_data, sizeof(read_data));)

#define APP_CHECK_DATA          (SEGGER_RTT_HasKey ())

void print_float (char *buffer, size_t buflen, double value);
bool compare_float (double val1, double val2);

#endif /* COMMON_UTILS_H_ */
