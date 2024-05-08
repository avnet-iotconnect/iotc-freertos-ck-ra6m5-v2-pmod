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
 * File Name    : console.c
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

/*
 * console.c
 *
 *  Created on: 15th November 2021
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "FreeRTOS.h"
#include "semphr.h"

#include "r_typedefs.h"
#include "hal_data.h"
#include "menu_thread.h"
#include "console.h"

#define MINIMUM_TOKEN_LENGTH      (5)   /* minimum length of a colour token */
#define MAXIMUM_TOKEN_LENGTH      (10)   /* maximum length of a colour token */
#define PRINT_BUFFER              (8*1024)
#define TRANSFER_LENGTH           (1024)

static uint8_t  g_out_of_band_received[TRANSFER_LENGTH];
static uint32_t g_transfer_complete = 0;
static uint32_t g_receive_complete  = 0;
static uint32_t g_out_of_band_index = 0;

uint32_t g_sci_spi0_mode = SCI_MODE;

/**********************************************************************************************************************
 * Function Name: open_console
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t open_console(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    /* Baud rate is handled in FSP */
    fsp_err = R_SCI_UART_Open(&g_console_uart_ctrl, &g_console_uart_cfg);
    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    return fsp_err;
}
/**********************************************************************************************************************
 End of function open_console
 *********************************************************************************************************************/


/*****************************************************************************
 * Function Name: get_colour
 *                Get the escape code string for the supplied colour tag
 * @param[in] char *string : the escape code
 * @param[out] bool_t *found : true if the tag was found, false if not
 * @retval the escape code for the colour tag, or the original string if there
 *         was no match
 ******************************************************************************/
static const char_t *get_colour(const char_t *string, bool_t *found)
{
    const char_t *p_colour_codes[] = {"[BLACK]", "\x1B[30m", "[RED]", "\x1B[91m", "[GREEN]", "\x1B[92m", "[YELLOW]",
                                    "\x1B[93m", "[BLUE]", "\x1B[94m", "[MAGENTA]", "\x1B[95m", "[CYAN]", "\x1B[96m",
                                    "[WHITE]", "\x1B[37m", "[ORANGE]", "\x1B[38;5;208m", "[PINK]", "\x1B[38;5;212m",
                                    "[BROWN]", "\x1B[38;5;94m", "[PURPLE]", "\x1B[35m"};
    uint8_t i;

    for (i = 0; i < 12; i++)
    {
        if (0 == strcmp(string, p_colour_codes[i << 1]))
        {
            *found = true;
            return p_colour_codes[(i << 1) + 1];
        }
    }

    *found = false;
    return (string);
}
/******************************************************************************
 * End of function get_colour
 ******************************************************************************/

/* ************************************************************************** */
/* Function Name: detokenise                                                  */
/*                Replace colour tokens with terminal colour escape codes     */
/* @param[in] input : input string possibly containing colour tokens          */
/* @param[out] output : string with colour tokens replaced with escape codes  */
/* @retval none                                                               */
/* ************************************************************************** */
static void detokenise(const char_t *input, char_t *output)
{
    int16_t         start_bracket_index;
    int16_t         end_bracket_index;
    int16_t         start_bracket_output_index;
    int16_t         i;
    int16_t         o;

    size_t          token_length;

    bool_t          token_found;
    bool_t          token_replaced;

    const char_t    *colour_code;

    char_t          token[MAXIMUM_TOKEN_LENGTH+1];

    start_bracket_index         = -1;
    end_bracket_index           = -1;
    start_bracket_output_index  = 0;
    o                           = 0;

    /* scan the input string */
    for (i = 0; '\0' != input[i]; i++)
    {
        token_replaced = false;

        /* token start? */
        if ('[' == input[i])
        {
            start_bracket_index = i;
            start_bracket_output_index = o;
        }

        /* token end? */
        if (']' == input[i])
        {
            end_bracket_index = i;

            /* check token */
            if (start_bracket_index >= 0)
            {
                token_length = (size_t)((end_bracket_index - start_bracket_index) + 1);

                if ((token_length >= MINIMUM_TOKEN_LENGTH) && (token_length <= MAXIMUM_TOKEN_LENGTH))
                {
                    /* copy the token to a buffer */
                    strncpy(token, &input[start_bracket_index], token_length);
                    token[token_length] = '\0';

                    /* check for a valid token */
                    colour_code = get_colour(token, &token_found);

                    /* if colour token exist, then replace it in the output with the associated escape code */
                    if (token_found)
                    {
                        strcpy(&output[start_bracket_output_index], colour_code);
                        o = (int16_t)(start_bracket_output_index + (int16_t)strlen(colour_code));
                        token_replaced = true;
                    }
                }
            }

            /* reset and start looking for another token */
            start_bracket_index = -1;
            end_bracket_index = -1;
        }

        /* if the token did not replace, then carry on copying input to output */
        if (!token_replaced)
        {
            output[o] = input[i];
            o++;
        }
    }

    /* terminate the output string */
    output[o] = '\0';
}
/******************************************************************************
 * End of function detokenise
 ******************************************************************************/

/* console_write */
/**********************************************************************************************************************
 * Function Name: console_write
 * Description  : .
 * Argument     : buffer
 * Return Value : .
 *********************************************************************************************************************/
void console_write(const char_t *buffer)
{
    fsp_err_t err = FSP_SUCCESS;

    g_transfer_complete = false;

    err = R_SCI_UART_Write(&g_console_uart_ctrl, (uint8_t *)buffer, strlen(buffer));

    assert (FSP_SUCCESS == err);

    while (!g_transfer_complete)
    {
        vTaskDelay(1);
    }
}
/**********************************************************************************************************************
 End of function console_write
 *********************************************************************************************************************/



static uint8_t s_rx_buf = 0;

/**********************************************************************************************************************
 * Function Name: start_key_check
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void start_key_check(void)
{
    s_rx_buf = 0;
    g_receive_complete = false;

    R_SCI_UART_Read(&g_console_uart_ctrl, &s_rx_buf, 1);

}
/**********************************************************************************************************************
 End of function start_key_check
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: get_detected_key
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
int8_t get_detected_key(void)
{
    return ((int8_t)s_rx_buf);
}
/**********************************************************************************************************************
 End of function get_detected_key
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: key_pressed
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
bool_t key_pressed(void)
{
    return (g_receive_complete);
}
/**********************************************************************************************************************
 End of function key_pressed
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: wait_for_keypress
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
char_t wait_for_keypress(void)
{
    uint8_t rx_buf = 0;

    if (true == g_receive_complete)
    {
        while (!g_receive_complete)
        {
            vTaskDelay (1);
        }
    }

    g_receive_complete = false;

    R_SCI_UART_Read(&g_console_uart_ctrl, &rx_buf, 1);

    while (!g_receive_complete)
    {
        vTaskDelay(1);
    }

    return ((char_t)rx_buf);
}
/******************************************************************************
 * End of function wait_for_keypress
 ******************************************************************************/

/**********************************************************************************************************************
 * Function Name: clear_console
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void clear_console(void)
{
    console_write("\x1b[2J\x1b[H");
}
/******************************************************************************
 * End of function clear_console
 ******************************************************************************/

#ifdef USE_DEBUG_CONSOLE
static void printf_colour_debug(const char_t *format, ...)
{
    va_list arglist;
    static char_t colour_format[PRINT_BUFFER];
    static char_t final_buffer[PRINT_BUFFER];

    va_start(arglist, format);

    /* replace colour tokens with terminal colour escape codes */
    detokenise(format, colour_format);

    vsprintf(final_buffer, colour_format, arglist);
    va_end(arglist);

    console_write(final_buffer);
}
#endif /* USE_DEBUG_CONSOLE */


/**********************************************************************************************************************
 * Function Name: printf_colour
 * Description  : As printf, but replaces colour tags with escape codes
 * Return Value : .
 *********************************************************************************************************************/
void printf_colour(const char_t *format, ...)
{
    va_list arglist;
    static char_t colour_format[PRINT_BUFFER];
    static char_t final_buffer[PRINT_BUFFER];

    if (g_console_out_mutex != NULL)
    {
        /* if the mutex obtained.  If the mutex is not available */
        /* wait 10 ticks to see if it becomes free. */
        if (xSemaphoreTakeRecursive(g_console_out_mutex, (TickType_t) 10) == pdTRUE)
        {
            /* the mutex can be obtain and now can access the shared resource. */

            va_start(arglist, format);

            /* replace colour tokens with terminal colour escape codes */
            detokenise(format, colour_format);

            vsprintf(final_buffer, colour_format, arglist);
            va_end(arglist);

#ifdef USE_DEBUG_INFO_CONSOLE
            TaskHandle_t h_task = xTaskGetCurrentTaskHandle();
            printf_colour_debug("[%p] ", h_task);
#endif /* USE_DEBUG_INFO_CONSOLE */

            console_write(final_buffer);

            xSemaphoreGiveRecursive(g_console_out_mutex);
            /* Now the mutex can be taken by other tasks. */
        }
        else
        {
            /* if the mutex cannot be obtain therefore not access */
            /* the shared resource safely. */
        }
    }
}
/******************************************************************************
 * End of function printf_colour
 ******************************************************************************/

/* callback from driver */
/**********************************************************************************************************************
 * Function Name: user_uart_console_callback
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void user_uart_console_callback(uart_callback_args_t *p_args)
{
    /* Handle the UART event */
    switch (p_args->event)
    {
        /* Received a character */
        case UART_EVENT_RX_CHAR:
        {
            /* Only put the next character in the receive buffer if there is space for it */
            if ((sizeof(g_out_of_band_received)) > g_out_of_band_index)
            {
                /* Write either the next one or two bytes depending on the receive data size */
                if (UART_DATA_BITS_8 >= g_console_uart_cfg.data_bits)
                {
                    g_out_of_band_received[g_out_of_band_index++] = (uint8_t) p_args->data;
                }
                else
                {
                    uint16_t * p_dest       =  (uint16_t *) &g_out_of_band_received[g_out_of_band_index];
                    *p_dest                 =  (uint16_t) p_args->data;
                    g_out_of_band_index     += 2;
                }
            }
            break;
        }

        /* Receive complete */
        case UART_EVENT_RX_COMPLETE:
        {
            g_receive_complete = 1;
            break;
        }

        /* Transmit complete */
        case UART_EVENT_TX_COMPLETE:
        {
            g_transfer_complete = 1;
            break;
        }
        default:
        {
        }
    }
}
/**********************************************************************************************************************
 End of function user_uart_console_callback
 *********************************************************************************************************************/



/* uart isr's */
extern void sci_uart_rxi_isr (void);
extern void sci_uart_txi_isr (void);
extern void sci_uart_tei_isr (void);
extern void sci_uart_eri_isr (void);

/**********************************************************************************************************************
 * Function Name: sci0_user_rxi_isr
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void sci0_user_rxi_isr(void)
{
    switch (g_sci_spi0_mode)
    {
        case SCI_MODE:
            sci_uart_rxi_isr();
            break;

        default:
            __asm__("nop");
    }
}
/**********************************************************************************************************************
 End of function sci0_user_rxi_isr
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: sci0_user_txi_isr
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void sci0_user_txi_isr(void)
{
    switch (g_sci_spi0_mode)
    {
        case SCI_MODE:
            sci_uart_txi_isr();
            break;
        case SPI_MODE:
        default:
            __asm__("nop");
    }
}
/**********************************************************************************************************************
 End of function sci0_user_txi_isr
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: sci0_user_tei_isr
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void sci0_user_tei_isr(void)
{
    switch (g_sci_spi0_mode)
    {
        case SCI_MODE:
            sci_uart_tei_isr();
            break;
        case SPI_MODE:
            break;
        default:
            __asm__("nop");
    }
}
/**********************************************************************************************************************
 End of function sci0_user_tei_isr
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: sci0_user_eri_isr
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void sci0_user_eri_isr(void)
{
    switch (g_sci_spi0_mode)
    {
        case SCI_MODE:
            sci_uart_eri_isr();
            break;
        case SPI_MODE:
            break;
        default:
            __asm__("nop");
    }
}
/**********************************************************************************************************************
 End of function sci0_user_eri_isr
 *********************************************************************************************************************/

