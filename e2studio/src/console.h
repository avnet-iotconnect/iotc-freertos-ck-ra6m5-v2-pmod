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
 * File Name    : console.h
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

/*
 * console.h
 *
 *  Created on: 15 Nov 2021
 *      Author: a5125422
 */

#ifndef CONSOLE_CONSOLE_H_
#define CONSOLE_CONSOLE_H_

#define UKN_MODE    (0)
#define SCI_MODE    (1)
#define SPI_MODE    (2)

extern uint32_t g_sci_spi0_mode;

#define SCI0_UNKONWN g_sci_spi0_mode = UKN_MODE
#define SCI0_MODE    g_sci_spi0_mode = SCI_MODE
#define SPI0_MODE    g_sci_spi0_mode = SPI_MODE

fsp_err_t open_console (void);
void console_write (const char *buffer);
void console_transmit_end (void);
void console_receive_end (void);
void printf_colour (const char *format, ...);
void start_key_check (void);
void clear_console (void);
char_t wait_for_keypress (void);
int8_t get_detected_key (void);
bool_t key_pressed (void);

#endif /* CONSOLE_CONSOLE_H_ */
