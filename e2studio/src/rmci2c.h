/**********************************************************************************************************************
 * File Name    : RmcI2C.h
 * Description  : Contains data structures and function declarations
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
 ************************************************************************************************************************
 * This code was created by Dan Allen, 2020, Renesas Electronics America. It is intended for demonstration purposes only, not
 * for production. No performance or suitability for any purpose including medical devices is guaranteed or claimed. For
 * questions and inquiries visit Renesas.com

 **********************************************************************************************************************/
#ifndef RMCI2C_H_
#define RMCI2C_H_

#include "hal_data.h"

extern void RmComDevice_init (void);
extern void ICM20948_Sensor_init ();
extern void RmComDevice_init_Icm (void);

extern void delay_Microseconds (uint32_t us);
extern void _delay (uint32_t ms);

extern void icm_comms_i2c_callback (rm_comms_callback_args_t *p_args);
extern fsp_err_t icp_rmcom_i2c_r (uint8_t reg, uint8_t *val, uint8_t num);
extern fsp_err_t icp_rmcom_i2c_w (uint8_t reg, uint8_t *val, uint8_t num);
extern void icp_comms_i2c_callback (rm_comms_callback_args_t *p_args);
extern void ms_delay (uint32_t ms);
extern void delay_microseconds (uint32_t us);
extern void rmcomdevice_init_icm ();
extern fsp_err_t rmcom_i2c_r (uint8_t reg, uint8_t *val, uint8_t num);
extern fsp_err_t rmcom_i2c_w (uint8_t reg, uint8_t *val, uint8_t num);

#endif /* RMCI2C_H_ */
