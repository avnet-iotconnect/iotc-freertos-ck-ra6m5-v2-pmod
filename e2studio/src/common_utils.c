/***********************************************************************************************************************
 * File Name    : common_utils.c
 * Description  : Contains data structures and functions used common to the AP
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
 ***********************************************************************************************************************/

#include "common_utils.h"

/* Flag for user callback */
static volatile uint8_t g_uart_event = RESET_VALUE;

/**********************************************************************************************************************
 * Function Name: uart_initialize
 * Description  : Initialize  UART.
 * Return Value : FSP_SUCCESS         Upon successful open and start of timer
 *                Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 *********************************************************************************************************************/
fsp_err_t uart_initialize(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize UART channel with baud rate 115200 */
    err = R_SCI_UART_Open (&g_uart_ctrl, &g_uart_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n**  R_SCI_UART_Open API failed  **\r\n");
    }
    return err;
}
/**********************************************************************************************************************
 End of function uart_initialize
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: uart_print_user_msg
 * Description  : Print user data on UART.
 * Argument     : p_msg
 * Return Value : FSP_SUCCESS Upon successful open and start of timer
 *                Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 *********************************************************************************************************************/
fsp_err_t uart_print_user_msg(uint8_t *p_msg)
{
    fsp_err_t   err             = FSP_SUCCESS;

    uint8_t     msg_len         = RESET_VALUE;
    uint32_t    local_timeout   = (DATA_LENGTH * UINT16_MAX);

    char_t        *p_temp_ptr     = (char_t *)p_msg;

    /* Calculate length of message received */
    msg_len = ((uint8_t)(strlen(p_temp_ptr)));

    /* Reset callback capture variable */
    g_uart_event = RESET_VALUE;

    /* Writing to terminal */
    err = R_SCI_UART_Write (&g_uart_ctrl, p_msg, msg_len);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n**  R_SCI_UART_Write API Failed  **\r\n");
        return err;
    }

    /* Check for event transfer complete */
    while ((UART_EVENT_TX_COMPLETE != g_uart_event) && (--local_timeout))
    {
        /* Check if any error event occurred */
        if (UART_ERROR_EVENTS == g_uart_event)
        {
            APP_ERR_PRINT ("\r\n**  UART Error Event Received  **\r\n");
            return FSP_ERR_TRANSFER_ABORTED;
        }
    }
    if (RESET_VALUE == local_timeout)
    {
        err = FSP_ERR_TIMEOUT;
    }
    return err;
}
/**********************************************************************************************************************
 End of function uart_print_user_msg
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: print_float
 * Description  : Print float.
 *                FSP_SUCCESS         Upon successful open and start of timer
 *                Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 * Arguments    : buffer
 *              : buflen
 *              : value
 * Return Value : .
 *********************************************************************************************************************/
void print_float(char_t *buffer, size_t buflen, double value)
{
    char_t    sign        = RESET_VALUE;
    int_t     integer     = (int) value;
    int_t     fraction    = (int) ((value-integer) * 100.0);

    if (NULL == buffer)
    {
        return;
    }
    if (value < 0.0)
    {
        sign = '-';
        snprintf (buffer, buflen, "%c%02d.%02d", sign, abs (integer), abs (fraction));
    }
    else
    {
        snprintf (buffer, buflen, "%02d.%02d", abs (integer), abs (fraction));
    }
}

/**************************************************************************************
 * Name:       delayMicroseconds
 * Function:   delay specified number of microseconds
 * Parameters: us (uint16_t) number of microseconds to delay
 * Return:     None
 **************************************************************************************/
void delayMicroseconds(uint32_t us)
{
    R_BSP_SoftwareDelay(us, BSP_DELAY_UNITS_MICROSECONDS);
}
/**********************************************************************************************************************
 End of function delayMicroseconds
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: delay
 * Description  : delay specified number of Milliseconds
 * Argument     : ms (uint16_t) number of Milliseconds to delay
 * Return Value : .
 *********************************************************************************************************************/
void delay(uint32_t ms)
{
    vTaskDelay(MILLISECONDS_TO_TICKS(ms));
}
/**********************************************************************************************************************
 End of function delay
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: Delay
 * Description  : delay specified number of microseconds
 * Argument     : xms (uint16_t) number of microseconds to delay
 * Return Value : .
 *********************************************************************************************************************/
void Delay(unsigned int xms)
{
    delay(xms);
}
/**********************************************************************************************************************
 End of function Delay
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: compare_float
 * Description  : Comapare floating point values
 * Arguments    : val1 (double), val2 (double)
 * Return Value : bool returns true if float point value is same
 *********************************************************************************************************************/
bool compare_float (double val1, double val2)
{
    float64_t precision = 0.0000001;

    if (fabs(val1 - val2) < precision)
    {
        return true;
    }
    else
    {
        return false;
    }
}
/**********************************************************************************************************************
 End of function compare_float
 *********************************************************************************************************************/


