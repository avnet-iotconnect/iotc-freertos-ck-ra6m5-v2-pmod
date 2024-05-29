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
 * File Name    : user_board_cfg.h
 * Description  : Board specific/configuration data.
 *********************************************************************************************************************/

#include "../ra/board/ra6m5_ck/board.h"

#include "bsp_api.h"
#include "hal_data.h"
#include "common_data.h"

#ifndef BOARD_CFG_
#define BOARD_CFG_

#define USB_EP_PACKET_SIZE                      (512U)
#define OFF                                     (0U)
#define ON                                      (1U)

#ifdef BOARD_RA6M5_CK

#define RED_LED       (BSP_LED_LED1)       /*LED1 RED*/
#define GREEN_LED     (BSP_LED_LED3)       /*LED3 GREEN*/
#define BLUE_LED      (BSP_LED_LED4)       /*LED4 BLUE*/

#define RGB_RED_LED   (BSP_LED_LED2_RED)   /*LED2_TRI_COLOUR_RED*/
#define RGB_GREEN_LED (BSP_LED_LED2_GREEN) /*LED2_TRI_COLOUR_GREEN*/
#define RGB_BLUE_LED  (BSP_LED_LED2_BLUE)  /*LED2_TRI_COLOUR_BLUE*/


#define TURN_RED_ON    R_BSP_PinAccessEnable(); R_BSP_PinWrite(g_bsp_leds.p_leds[RED_LED], ON); R_BSP_PinAccessDisable();
#define TURN_RED_OFF   R_BSP_PinAccessEnable(); R_BSP_PinWrite(g_bsp_leds.p_leds[RED_LED], OFF); R_BSP_PinAccessDisable();
#define TURN_GREEN_ON  R_BSP_PinAccessEnable(); R_BSP_PinWrite(g_bsp_leds.p_leds[GREEN_LED], ON); R_BSP_PinAccessDisable();
#define TURN_GREEN_OFF R_BSP_PinAccessEnable(); R_BSP_PinWrite(g_bsp_leds.p_leds[GREEN_LED], OFF); R_BSP_PinAccessDisable();

#define TURN_BLUE_ON   R_BSP_PinAccessEnable(); R_BSP_PinWrite(g_bsp_leds.p_leds[BLUE_LED], ON); R_BSP_PinAccessDisable();
#define TURN_BLUE_OFF  R_BSP_PinAccessEnable(); R_BSP_PinWrite(g_bsp_leds.p_leds[BLUE_LED], OFF); R_BSP_PinAccessDisable();

#define SYSTEM_OK      R_BSP_PinAccessEnable(); TURN_GREEN_ON; TURN_RED_OFF; R_BSP_PinAccessDisable();
#define SYSTEM_ERROR   R_BSP_PinAccessEnable(); TURN_GREEN_OFF; TURN_RED_ON; R_BSP_PinAccessDisable(); APP_ERR_TRAP(1);

#endif /* BOARD_RA2L1_EK */

extern bsp_leds_t g_bsp_leds;

#endif /* BOARD_CFG_ */