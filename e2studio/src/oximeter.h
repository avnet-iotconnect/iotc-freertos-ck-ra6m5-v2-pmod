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
* This code was created 2021 by Renesas Electronics America. It is intended for demonstration purposes only, not
* for production. No performance or suitability for any purpose including medical devices is guaranteed or claimed. For
* questions and inquiries visit Renesas.com
*/
/**********************************************************************************************************************
 * File Name    : oximeter.h
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/


#ifndef OXIMETER_H_
#define OXIMETER_H_

#include <stdint.h>
#include "SPO2/SPO2.h"

#define LOW_AC_PERSIST (10)

#define BUSY                (1)
#define NOT_BUSY            (0)
#define DEMO_MODE           (0)
#define CALIBRATION_MODE    (1)
#define BUSY                (1)
#define NOT_BUSY            (0)
#define POWER_ON_PIN        (BSP_IO_PORT_01_PIN_07)


#define PROX_DELAY               (100)
#define SAMPLES_TO_SKIP_FOR_UART (0)

extern uint8_t mode;
extern uint8_t no_disp_spo2;

#define MEAS_PS             (1)


extern struct oxim ox;
extern void defaultConfig (struct oxim *_ox, struct ob1203 *_ob); //populate the default settings here
extern void ob1203_spo2_main (struct oxim * _ox, struct spo2 * _sp, struct ob1203 * _ob);
extern void main_init (struct oxim * _ox, struct spo2 * _sp, struct ob1203 * _ob);
extern void proxEvent (void);
extern void dataEvent (void);
extern void IRQ_Disable (void);
extern void IRQ_Enable (void);

extern void switch_mode (struct oxim *_ox, struct ob1203 *_ob, struct spo2 *_sp, uint8_t prox_bio_mode);
extern void reset_oximeter_params (struct oxim *_ox);

extern void get_sensor_data (struct oxim *_ox, struct ob1203 *_ob, struct spo2 *_sp);

extern void setup_calibration_mode (void);
extern void delayMicroseconds (uint32_t us);
extern void do_segger_print (struct oxim * _ox,struct spo2 * _sp);
#endif /* OXIMETER_H_ */
