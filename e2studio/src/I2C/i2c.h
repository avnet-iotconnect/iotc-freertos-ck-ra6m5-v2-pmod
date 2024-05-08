/***********************************************************************************************************************
 * File Name    : i2c.h
 * Description  : Contains functions declarations
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
*
************************************************************************************************************************
* This code was created by Dan Allen, 2020, Renesas Electronics America. It is intended for demonstration purposes only, not
* for production. No performance or suitability for any purpose including medical devices is guaranteed or claimed. For
* questions and inquiries visit Renesas.com

**********************************************************************************************************************/

#ifndef I2C_H
#define I2C_H


#define DEL (NOP(); NOP(); NOP(); NOP();)

/* I2C write */
fsp_err_t I2C_w (uint8_t addr, uint8_t reg, uint8_t * val, char num);

/* I2C read write */
fsp_err_t I2C_r (uint8_t addr, uint8_t reg, uint8_t * val, char num);

/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif
