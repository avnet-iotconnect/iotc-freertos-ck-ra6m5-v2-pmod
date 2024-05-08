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
 * File Name    : oximeter.c
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <stdbool.h>
#include "OB1203/OB1203.h"
#include "KALMAN/kalman.h"
#include "SPO2/SPO2.h"
#include "oximeter.h"
#include "I2C/i2c.h"
#include "hal_data.h"
#include "common_utils.h"
#include "user_choice.h"
#include "common_init.h"

/******************************************************
Set configurations in oximstruct.h
*******************************************************/
struct oxim ox;

extern kalman_t kalman_filters[NUM_KALMAN_FILTERS];

unsigned char g_adc_completed = 0;

volatile uint8_t            samples_ready   = 0;
volatile uint8_t            just_woke_up    = 0;
volatile unsigned char      uart2_busy_flag;
volatile uint8_t            uart2_receive_complete_flag;
uint8_t                     mode = PROX_MODE;
uint8_t                     no_disp_spo2 = 0;
void                        (*p_IntB_Event)(void) = NULL;
char_t                        comment[30];
char_t                        tx_buffer[64];
uint8_t                     UART_Plot = 1;
uint16_t                    spo2_val; //value output to display
uint16_t                    heart_Rate_Val; //value output to display
uint16_t                    breathing_rate;
uint16_t                    r_p2p;
static volatile uint32_t    total_time  = 0;
static volatile uint16_t    t_time      = 0;

static bool_t OB_1203_calibrated = false;
static bool_t OB_1203_sensing    = false;

static uint8_t status_reg_contents[2] = {0U};

void do_uart_print (void);
bool_t r_ob1203_get_sensing_status (st_sensorsOB_t *p_data);

void check_for_alg_reset (struct oxim *_ox, struct ob1203 *_ob,struct spo2 *_sp);


/**********************************************************************************************************************
 * Function Name: oxim_struct_init
 * Description  : .
 * Argument     : _ox
 * Return Value : .
 *********************************************************************************************************************/
static void oxim_struct_init(struct oxim *_ox)
{
    _ox->sample_log         = 0;
    _ox->samples_processed  = 0;
    _ox->update_display     = 0;
    _ox->update_ppg_wave    = 0;
    _ox->count              = 0;
    _ox->clear              = 0;
    _ox->overflow           = 0; //missed sample count is global scope for debugging_ox->`
    _ox->Change_Block       = 0;
    _ox->fifo_mode          = 0;
    _ox->heart_beat         = 0;
    _ox->demo_stop          = 0;

    /*Set to MIN_TARGET-2^16. Minimum biosensor ADC count.
     * Below this sensor exits BIO mode and reverts to proximity sensor "wait for finger" mode */
    _ox->BIO_THRESHOLD      = 0x4000;
    _ox->MAX_LOW_SAMPLES    = DEFAULT_MAX_LOW_SAMPLES;
    _ox->delay_adjust       = 0;
    _ox->mode               = PROX_MODE; //start in prox mode, then switch to HR mode when detect the proximity
    _ox->idle_counter       = 0;
    _ox->num_low_samples    = 0;
    _ox->sample_cnt         = 0;
    _ox->reset_alg          = 0;
    _ox->mode               = 0;
#ifdef HR_ONLY
    _ox->red_agc = 0;
    _ox->ppg2 = 0;
#else
    _ox->red_agc = 1; //default to spo2 mode
    _ox->ppg2 = 1; //default to spo2 mode
#endif
    _ox->ir_agc = 1;

}



/**********************************************************************************************************************
 * Function Name: defaultConfig
 * Description  : Function:  Configure OB1203 registers. This function gives visibility into the
 *                OB1203 register settings.
 * Arguments    : _ox
 *              : _ob
 * Return Value : .
 *********************************************************************************************************************/
void defaultConfig(struct oxim *_ox, struct ob1203 *_ob) //populate the default settings here
{

#ifndef LEAN_OB1203
    temp_en = TEMP_OFF;     //temperature sensor settings (hidden registers)
    ls_en = LS_OFF;
    //PS and PPG settings
    ps_sai_en = PS_SAI_OFF;
    ps_pulses = PS_PULSES(3);
    ps_pwidth = PS_PWIDTH(1);
    ps_rate = PS_RATE(5); //5 = 100ms
    ps_avg_en = PS_AVG_OFF;
    ps_can_ana = PS_CAN_ANA_0;
    ps_digital_can = 0;
    ps_hys_level = 0;
#endif
    if (MEAS_PS)
    {
        _ob->ps_current = 0x100;
    }
    else
    {
        _ob->ps_current = 0x000;
    }

    _ob->ps_thres_hi = PS_THRESH_HI;
    _ob->ps_thres_lo = 0x00;

    /*interrupts */
#ifndef LEAN_OB1203
    _ob->ls_int_sel = LS_INT_SEL_W;
    _ob->ls_var_mode = LS_THRES_INT_MODE;
    _ob->ls_int_en = LS_INT_OFF;
    _ob->ls_persist = LS_PERSIST(2);
    _ob->ps_persist = PS_PERSIST(2);
    _ob->ps_logic_mode = PS_INT_READ_CLEARS;
    _ob->ppg_pwidth = PPG_PWIDTH(3);                //PPG_PWIDTH(3);
#endif
    _ob->ppg_ps_en = PPG_PS_ON;
    _ob->ps_int_en = PS_INT_OFF; //turn it on later after display boot.

    /*BIO SETTINGS */
    /*int */
    _ob->afull_int_en   = AFULL_INT_OFF;
    _ob->ppg_int_en     = PPG_INT_ON;

    /*PPG */
    _ob->ir_current = 0x2FF; //max 1023. 3FF was 1AF
    if (_ox->ppg2)
    {
        _ob->r_current = 0x1AF; //max 511. 1FF was 1AF
    }
    else
    {
        _ob->r_current = 0;
    }
    _ob->ppg_ps_gain = PPG_PS_GAIN_1;
#ifndef LEAN_OB1203
    _ob->ppg_pow_save = PPG_POW_SAVE_OFF;
    _ob->led_flip = LED_FLIP_OFF;
    _ob->ch1_can_ana = PPG_CH1_CAN(0);
    _ob->ch2_can_ana = PPG_CH2_CAN(0);
#endif
#ifndef _50sps
    #ifdef BEST_SNR
    _ob->ppg_period = PPG_PERIOD(1); //1600 SPS
    _ob->ppg_avg = PPG_AVG(4);    //16 averages to 100 SPS output data rate for IR and R
    #endif
    #ifdef MID_POWER
    _ob->ppg_period = PPG_PERIOD(3); //800 SPS
    _ob->ppg_avg = PPG_AVG(3);//8 averages to 100 SPS output data rate for IR and R
    #endif

    #ifdef LOW_POWER
    _ob->ppg_period = PPG_PERIOD(4); //400 SPS
    _ob->ppg_avg    = PPG_AVG(2);    //4 averages to 100 SPS output data rate for IR and R
    #endif

    #ifdef LOW_POWER_2
    _ob->ppg_period = PPG_PERIOD(5); //200 SPS
    _ob->ppg_avg = PPG_AVG(1);    //2 averages to 100 SPS output data rate for IR and R
    #endif

    #ifdef LOW_POWER_3
    _ob->ppg_period = PPG_PERIOD(6); //100 SPS
    _ob->ppg_avg = PPG_AVG(0);    //1 averages to 100 SPS output data rate for IR and R
    #endif


#else //_50sps
    #ifdef BEST_SNR
    _ob->ppg_period = PPG_PERIOD(1); //1600 SPS
    _ob->ppg_avg = PPG_AVG(5);    //32 averages to 50 SPS output data rate for IR and R
    #endif

    #ifdef MID_POWER
    _ob->ppg_period = PPG_PERIOD(3); //800 SPS
    _ob->ppg_avg = PPG_AVG(4);    //16 averages to 50 SPS output data rate for IR and R
    #endif

    #ifdef LOW_POWER
    _ob->ppg_period = PPG_PERIOD(4); //400 SPS
    _ob->ppg_avg = PPG_AVG(3);    //8 averages to 50 SPS output data rate for IR and R
    #endif

    #ifdef LOW_POWER_2
    _ob->ppg_period = PPG_PERIOD(5); //200 SPS
    _ob->ppg_avg = PPG_AVG(2);    //4 averages to 50 SPS output data rate for IR and R
    #endif

    #ifdef LOW_POWER_3
    _ob->ppg_period = PPG_PERIOD(6); //100 SPS
    _ob->ppg_avg = PPG_AVG(1);    //2 averages to 50 SPS output data rate for IR and R
    #endif

    #ifdef LOW_POWER_4
    _ob->ppg_period = PPG_PERIOD(7); //50 SPS
    _ob->ppg_avg = PPG_AVG(0);    //0 averages to 50 SPS output data rate for IR and R
    #endif

#endif


#ifndef LEAN_OB1203
    _ob->ppg_freq = PPG_FREQ_PRODUCTION;         //sets the data collection rate to multiples of 50Hz.
    _ob->bio_trim = 3;                           //max 3 --this dims the ADC sensitivity, but reduces noise
    _ob->led_trim = 0x00;                        //can use to overwrite trim setting and max out the current
    _ob->ppg_LED_settling = PPG_LED_SETTLING(2); //hidden register for adjusting LED setting time
                                                    //(not a factor for noise)
    _ob->ppg_ALC_track = PPG_ALC_TRACK(2);       //hidden register for adjusting ALC track and hold time
                                                    //(not a factor for noise)
    _ob->diff = DIFF_ON;                         //hidden register for turning off subtraction of residual
                                                    //ambient light after ALC
    _ob->alc = ALC_ON;                           //hidden register for turning off ambient light cancellation
                                                    //track and hold circuit
    _ob->sig_out = SIGNAL_OUT;                   //hidden register for selecting ambient sample or LED sample
                                                    //if DIFF is off
    _ob->fifo_rollover_en = FIFO_ROLL_ON;
    _ob->fifo_afull_advance_warning = AFULL_ADVANCE_WARNING(0x0C); //balance early warning versus large sample count
#endif
    /*run initialization according to user compile settings */
    uint8_t reg_data[2];
    i2c_r(OB1203_ADDR, REG_DIG_LED1_TRIM, reg_data, 2);
    _ob->led1_orig_trim = reg_data[0];
    _ob->led2_orig_trim = reg_data[1];

    if (BIO_MODE == _ox->mode)
    {
        _ox->ppg2 ? init_spo2(_ob) : init_hr(_ob);
    }
    else
    {
        _ob->ppg_int_en = PPG_INT_OFF;
        init_ps(_ob);
    }
}

/**********************************************************************************************************************
 * Function Name: reset_oximeter_params
 * Description  : .
 * Argument     : _ox
 * Return Value : .
 *********************************************************************************************************************/
void reset_oximeter_params(struct oxim *_ox)
{
    _ox->samples_processed  = 0;
    _ox->sample_cnt         = 0;
    _ox->do_alg             = 0;
    _ox->update_display     = 0;
    _ox->update_ppg_wave    = 0;
    _ox->sample_log         = 0;
    _ox->heart_beat         = 0;
}
/**********************************************************************************************************************
 End of function reset_oximeter_params
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: check_for_alg_reset
 * Description  : .
 * Arguments    : _ox
 *              : _ob
 *              : _sp
 * Return Value : .
 *********************************************************************************************************************/
void check_for_alg_reset(struct oxim *_ox, struct ob1203 *_ob,struct spo2 *_sp)
{
    if (1 == _ox->reset_alg)
    {
        SPO2_init(_ox, _ob, _sp);
        reset_oximeter_params(_ox);
        _sp->avg_PI = get_avg_PI(0, &_sp->pi_rms_val);
        _ox->reset_alg = 0;
    }
}
/**********************************************************************************************************************
 End of function check_for_alg_reset
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: r_ob1203_get_sensing_status
 * Description  : .
 * Argument     : p_data
 * Return Value : .
 *********************************************************************************************************************/
bool_t r_ob1203_get_sensing_status(st_sensorsOB_t *p_data)
{
    p_data->ob_spo2 = spo2_val;
    p_data->ob_hr   = heart_Rate_Val;
    p_data->ob_rr   = breathing_rate;
    p_data->ob_pi   = 0.0;

    if ((true == OB_1203_calibrated) & (0 != heart_Rate_Val))
    {
        OB_1203_calibrated = true;
        OB_1203_sensing    = false;
    }
    else
    {
        heart_Rate_Val     = 0;
    }

    p_data->calibrated = OB_1203_calibrated;
    p_data->sensing    = OB_1203_sensing;
    return (true);
}
/**********************************************************************************************************************
 End of function r_ob1203_get_sensing_status
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: user_uart_callback
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void user_uart_callback(uart_callback_args_t *p_args)
{
    if (UART_EVENT_RX_COMPLETE == p_args->event)
    {
        /* Do nothing */
    }
    uart2_busy_flag = NOT_BUSY;
}
/**********************************************************************************************************************
 End of function user_uart_callback
 *********************************************************************************************************************/



/************************************************************************************
 * Name:       get_sensor_data
 * Function:   reads data from the OB1203 prox register or PPG FIFO data
 * Parameters: none
 * Return:     latest IR data (for PPG waveform display)
 ************************************************************************************/
/**********************************************************************************************************************
 * Function Name: get_sensor_data
 * Description  : .
 * Arguments    : _ox
 *              : _ob
 *              : _sp
 * Return Value : .
 *********************************************************************************************************************/
void get_sensor_data(struct oxim *_ox, struct ob1203 *_ob, struct spo2 *_sp)
{
    uint8_t maxSamples2Read;
    uint8_t total_samples;
    if (_ox->ppg2)
    {
        /*FIFO samples, e.g. 4 samples * 3 bytes = 12 bytes (or 2 SpO2 samples) 16 samples is the entire SpO2 buffer.*/
        maxSamples2Read = 10;

    }
    else
    {
        maxSamples2Read = 20; /*in HR mode, collect twice as many samples and get AFUL interrupts half as often*/
    }

    uint8_t fifoBuffer[16 * 6];
    uint8_t sample_info[3];
    uint8_t fifo_reg_data[3];
    uint8_t samples2Read  = 0;
    uint8_t do_reset_fifo = 0;

    if (_ob->afull_int_en)
    {
        /*FIFO mode--find out how many samples in buffer*/
        /*read the samples fifo registers and figure out how many samples are left*/
        getNumFifoSamplesAvailable (_ob, fifo_reg_data, sample_info);
        if (_ox->ppg2)
        {
            samples2Read =
                    (sample_info[1] > maxSamples2Read) ?
                        maxSamples2Read : sample_info[1]; /*limit the number of samples to the maximum*/
        }
        else
        {
            /*limit the number of samples to the maximum and makes sure it is even number*/
            samples2Read =
                    ((sample_info[1] << 1) > maxSamples2Read) ?
                        maxSamples2Read : (sample_info[1] << 1);
        }
        _ox->overflow = sample_info[2];
    }
    else
    {
        samples2Read = 1; /*read one sample*/
        _ox->overflow = 0;
    }
    if (_ox->ppg2)
    {
        total_samples = samples2Read << 1;
    }
    else
    {
        total_samples = samples2Read;
    }
    getFifoSamples(total_samples, fifoBuffer);
    parseFifoSamples(total_samples, fifoBuffer, _ox->ppgData);

    _ox->latest_ir_data = _ox->ppgData[0];
    if (_ox->ppg2)
    {

        _ox->latest_r_data = _ox->ppgData[1];
    }
    else
    {
        _ox->latest_r_data = 0;
    }

    /*IR in range, and if spo2 mode then R must be in range too*/
    if (_ob->ir_in_range && ((_ox->ppg2 && _ob->r_in_range) || (0 == _ox->ppg2)))
    {
        /*Fill in missing samples from FIFO overflow (doesn't normally run)*/
        if (_ox->ppg2) /*SPO2 mode*/
        {
            for (int_t n = 0; n < (_ox->overflow >> 1); n++)
            {
                {
                    /*duplicate oldest data to deal with missing (overwritten) samples*/
                    add_sample(_ob, _sp, _ox->ppgData[0], _ox->ppgData[1]);
                }
                if (_sp->sample_count < ARRAY_LENGTH)
                {
                    _sp->sample_count++;
                } /*number of samples in buffer*/
                _sp->missed_sample_count++;
            }
        }
        else /*HR only mode*/
        {
            for (int_t n = 0; n < _ox->overflow; n++)
            {
                /*duplicate oldest data to deal with missing (overwritten) samples*/
                add_sample(_ob, _sp, _ox->ppgData[0], 0);
                if (_sp->sample_count < ARRAY_LENGTH)
                {
                    _sp->sample_count++;
                } /*number of samples in buffer*/
                _sp->missed_sample_count++;
            }
        }

        /*Load collected samples (normally runs)*/
        for (int_t n = 0; n < samples2Read; n++)
        {/*add samples*/
            if (_ox->ppg2)
            {
                /*add data to sample buffer when data is in range*/
                add_sample(_ob, _sp, _ox -> ppgData[2 * n], _ox->ppgData[(2 * n) + 1]);
            }
            else
            {
                add_sample(_ob, _sp, _ox->ppgData[n], 0); /*add data to sample buffer when data is in range*/
            }
            if (_sp->sample_count < ARRAY_LENGTH)
            {
                _sp->sample_count++;
            } /*number of samples in buffer*/
            _sp->read_sample_count++;
        }
    }
    else
    {
        _sp->sample_count = 0;
    }

    if ((_ox->latest_r_data > 8000) && (_ox->delay_adjust < 10))
    {
        _ox->delay_adjust++;
    }

    if ((10 == _ox->delay_adjust) || (0 == _ox->ppg2))
    {
        /*red LED can heat up so let it warm up and droop before AGC, and allow regardless if ppg1 mode*/
        /* R AGC case */
        if (_ox->ppg2 && _ox->red_agc)
        {

            /*use the most recent sample in the FIFO*/
            do_agc(_ob, _ox->ppgData[(2 * (samples2Read - 1)) + 1], 1, _ox->ppg2);
        }

        /* IR AGC case*/
        if (_ox->ir_agc)
        {
            if (_ox->ppg2)
            {
                /*use the most recent sample in the FIFO*/
                do_agc(_ob, _ox->ppgData[2 * (samples2Read - 1)], 0, _ox->ppg2);
            }
            else
            {
                do_agc(_ob, _ox->ppgData[samples2Read - 1], 0, _ox->ppg2); /*use the most recent sample in the FIFO*/
            }
        }
    }

    if (_ob->update_fifo_mode || _ob->update_current)
    {
        do_reset_fifo = 1;
    }
    if (_ob->update_current)
    {
        if (_ox->fifo_mode)
        {
            _ox->reset_alg = 1;
        }
    }
    if (_ob->target_change)
    {
        if ((IR_START_CURRENT != _ob->ir_current) || ((1 == _ox->ppg2) && (R_START_CURRENT != _ob->r_current)))
        { /*if this is not the initial startup*/
            if (_ox->fifo_mode)
            { /*logging data*/
                _ox->reset_alg = 1;
            }
        }
        _ob->target_change = 0;
    }
    if (_ob->update_fifo_mode)
    {
        setIntConfig(_ob);
        _ob->update_fifo_mode = 0;
        if (0 == _ob->ppg_int_en)
        { /*not in fifo mode -- switching to collecting data*/
            _ox->fifo_mode = 1;
        }
        else
        {
            _ox->fifo_mode = 0;
        }
    }
    if (_ob->update_current)
    {
        setPPGcurrent(_ob);
        _ob->update_current = 0;
    }
    if (_ob->ppg_gain_change)
    {
        setPPG_PSgain_cfg(_ob);
        _ob->ppg_gain_change = 0;
    }
    if (do_reset_fifo)
    { //reset the FIFO unless that is going to reset
#ifndef UPDATE_DISPLAY
        resetFIFO(_ob);
#else
        if (!_ox->reset_alg)
        { //no need to reset if the algorithm is going to reset
            resetFIFO(_ob);
        }
#endif
        do_reset_fifo = 0;
    }

    /*switch modes if low signal*/
    uint8_t last_ir_sample_index = (1 == _ox->ppg2) ? (2 * (samples2Read-1)) : (samples2Read-1);

    if (_ox->ppgData[last_ir_sample_index] < _ox->BIO_THRESHOLD)
    { // counts low samples when num_low_samples exceeds max low samples mode will switch to prox mode
        _ox->num_low_samples++;
    }
    else
    {
        _ox->num_low_samples = 0;
    }

    /* here the prox sensor starts / stops the measurement */
    if (_ox->num_low_samples >= _ox->MAX_LOW_SAMPLES)
    {
        _ox->mode = PROX_MODE;
        IRQ_Disable();
        p_IntB_Event = &proxEvent; //set an interrupt on the falling edge
        IRQ_Enable();
        switch_mode(_ox, _ob, _sp, _ox->mode);

        just_woke_up        = 0;
        OB_1203_calibrated  = false;
        OB_1203_sensing     = false;
    }
    samples_ready = 0;
}
/**********************************************************************************************************************
 End of function get_sensor_data
 *********************************************************************************************************************/



volatile uint16_t t_timer = 0;

/**********************************************************************************************************************
 * Function Name: t_callback
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void t_callback(timer_callback_args_t *p_args)
{
    /* Called periodically from ISR of timing unit */
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        t_time++;
        total_time++;
        t_timer++;
    }
;
}
/**********************************************************************************************************************
 End of function t_callback
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: proxEvent
 * Description  : ISR for OB1203 interrupt in proximity mode
 * Return Value : .
 *********************************************************************************************************************/
void proxEvent(void)
{
    just_woke_up = true;
}
/**********************************************************************************************************************
 End of function proxEvent
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: dataEvent
 * Description  : ISR for OB1203 interrupt in bio mode
 * Return Value : .
 *********************************************************************************************************************/
void dataEvent(void)
{
    samples_ready = true;
}
/**********************************************************************************************************************
 End of function dataEvent
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: IRQ_Disable
 * Description  : Disable Interrupt for sensor pin
 * Return Value : .
 *********************************************************************************************************************/
void IRQ_Disable(void)
{
    fsp_err_t status = FSP_SUCCESS;
    status = R_ICU_ExternalIrqDisable (&g_sensorIRQ_ctrl);
    if (FSP_SUCCESS != status)
    {
        APP_PRINT("** R_ICU_ExternalIrqDisable failed ** %u \r\n",status);
    }
}
/**********************************************************************************************************************
 End of function IRQ_Disable
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: IRQ_Enable
 * Description  : Enable Interrupt for sensor pin
 * Return Value : .
 *********************************************************************************************************************/
void IRQ_Enable(void)
{
    fsp_err_t status = FSP_SUCCESS;
    status = R_ICU_ExternalIrqEnable (&g_sensorIRQ_ctrl);
    if (FSP_SUCCESS != status)
    {
        APP_PRINT ("** R_ICU_ExternalIrqEnable failed ** %u \r\n",status);
    }
    else
    {
        APP_PRINT ("** R_ICU_ExternalIrqEnable success ** %u \r\n",status);
    }
}
/**********************************************************************************************************************
 End of function IRQ_Enable
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: do_segger_print
 * Description  : .
 * Arguments    : _ox
 *              : _sp
 * Return Value : .
 *********************************************************************************************************************/
void do_segger_print(struct oxim * _ox,struct spo2 * _sp)
{
    if ((UART_Plot) && (_ox->latest_ir_data > 8000))
    {                                     //wait until not busy
        memset(tx_buffer, 0, sizeof(_ox->tx_buffer)); //zero the send buffer
        if (0 == _sp->display_spo2)
        {
            sprintf(tx_buffer, "%ld"
                    ", "
                    "SpO2 HR  RR  PI   R\r\n",
                    total_time);
            sprintf(comment, "\r\n");
        }
        else /*if (ir_peak_norm >= MIN_RMS)*/
        {
            sprintf(tx_buffer, "%d"  ", " "%d"   ","  "%u"   "," "%.3f",
                    (uint16_t) (round_dec_spo2(_sp->display_spo2)), _sp->display_hr, _sp->br, _sp->R);
            spo2_val = (uint16_t)(_sp->display_spo2 / 10);
            heart_Rate_Val  = _sp->display_hr;
            breathing_rate  = _sp->br;
            r_p2p           = (uint16_t) _sp->R;

#ifdef OB1203_SENSOR_ENABLE
            APP_PRINT("\r\n OB1203 SPO2 DATA \r\n = %d \r\n", spo2_val);
            APP_PRINT("\r\n OB1203 HEART RATE DATA \r\n = %d \r\n",heart_Rate_Val);
            APP_PRINT("\r\n OB1203 BREATHING RATE DATA \r\n= %d\r\n",breathing_rate);
            APP_PRINT("\r\n OB1203 pEAK TO PEAK DATA \r\n= %d\r\n",r_p2p);
#endif
        }

#ifndef CHECK_PI
        sprintf(comment, "\r\n");
#endif

    }
}
/**********************************************************************************************************************
 End of function do_segger_print
 *********************************************************************************************************************/





/**********************************************************************************************************************
 * Function Name: switch_mode
 * Description  : changes OB1203 from low power proximity sensor mode to PPG sensing bio mode
 * Arguments    : _ox
 *              : _ob
 *              : _sp
 *              : prox_bio_mode
 * Return Value : .
 *********************************************************************************************************************/
void switch_mode(struct oxim *_ox, struct ob1203 *_ob, struct spo2 *_sp, uint8_t prox_bio_mode)
{
    if (BIO_MODE == prox_bio_mode)
    { //switch to bio sensor mode
        _ob->afull_int_en   = AFULL_INT_OFF;
        _ob->ppg_int_en     = PPG_INT_ON;
        _ox->fifo_mode      = 0;
        SPO2_init (_ox, _ob, _sp); /*completely reset the algorithm and OB1203 Fifo*/

        /*reset LED currents to 75% of maximum */
        _ob->ir_current = IR_START_CURRENT;
        if (0 == _ox->ppg2)
        {
            _ob->r_current = 0;
        }
        else
        {
            _ob->r_current = R_START_CURRENT;
        }
        _ob->target_change   = 1;
        _ob->ppg_gain_change = 0;
        if (_ox->ppg2)
        {
            init_spo2(_ob); /*initialize the OB1203 sensor in spo2 mode (includes a reset command)*/
        }
        else
        {
            init_hr(_ob);
        }

        _ob->target_counts[0] = 0; //reset AGC
        _ob->target_counts[1] = 0;
        _ob->led1_trim        = 0x00; //overwrite default prox sensor trim--not used for PPG mode
        _ob->led2_trim        = 0x00; //overwrite default prox sensor trim--not used for PPG mode
        setDigTrim(_ob);
        reset_oximeter_params(_ox);
        _ox->num_low_samples = 0;
    }
    else
    { //switch to proximity sensor mode
        _ob->afull_int_en = AFULL_INT_OFF;
        _ob->ppg_int_en   = PPG_INT_OFF;
        _ob->ps_int_en    = PS_INT_ON;
        _ob->ppg_ps_mode  = PS_MODE;
        _ob->ppg_ps_en    = PPG_PS_OFF;
        setMainConfig(_ob);
        init_ps(_ob);
        _ob->led1_trim = _ob->led1_orig_trim; //overwrite default prox sensor trim--not used for PPG mode
        _ob->led2_trim = _ob->led2_orig_trim; //overwrite default prox sensor trim--not used for PPG mode
        setDigTrim(_ob);
        _sp->br = 0;
        _sp->perfusion_index[RED] = 0;
        _sp->perfusion_index[IR] = 0;
    }
}
/**********************************************************************************************************************
 End of function switch_mode
 *********************************************************************************************************************/




/**********************************************************************************************************************
 * Function Name: main_init
 * Description  : tidying up function including all the init functions for OB1203, filters,
 *                  etc.
 * Arguments    : _ox
 *              : _sp
 *              : _ob
 * Return Value : .
 *********************************************************************************************************************/
void main_init(struct oxim * _ox, struct spo2 * _sp, struct ob1203 * _ob)
{
    oxim_struct_init(_ox);
    ob1203_struct_init(_ob);
    OB1203_init(OB1203_ADDR);
    init_kalman(kalman_filters); //must run before SPO2_init()
    SPO2_init(_ox, _ob, _sp); //run after init_kalman()
    defaultConfig(_ox, _ob); //do the OB1203 configuration now

    p_IntB_Event = &proxEvent; //set interrupt to call proxEvent ISR
    IRQ_Enable();
    _ob->ps_int_en = PS_INT_OFF;
    setIntConfig(_ob);
    _ob->ppg_ps_en = PPG_PS_OFF;
    setMainConfig(_ob);
    _ob->ps_int_en = PS_INT_ON;
    setIntConfig(_ob);
    uint8_t reg_data[20];
    i2c_r(OB1203_ADDR, 0x2B, reg_data, 3);
    _ob->ppg_ps_en = PPG_PS_ON;
    setMainConfig(_ob);

}
/**********************************************************************************************************************
 End of function main_init
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: ob1203_spo2_main
 * Description  : .
 * Arguments    : _ox
 *              : _sp
 *              : _ob
 * Return Value : .
 *********************************************************************************************************************/
void ob1203_spo2_main(struct oxim * _ox, struct spo2 * _sp, struct ob1203 * _ob)
{
    /*main program loop */
    if (PROX_MODE == _ox->mode)
    {
        /*prox mode case*/
        i2c_r(OB1203_ADDR, REG_STATUS_0, status_reg_contents, 2); /* Must read this register to clear interrupt state */

        /*******put sleep command here and register wake on interrupt******/
        xSemaphoreTake(g_ob1203_semaphore, portMAX_DELAY);

        if (just_woke_up)
        {
            _ox->mode = BIO_MODE;
            IRQ_Disable();
            p_IntB_Event = &dataEvent; //attach interrupt to data events
            IRQ_Enable();
            switch_mode (_ox, _ob, _sp, _ox->mode); //starts in PPG fast mode
            just_woke_up        = 0;
            OB_1203_calibrated  = true;
            OB_1203_sensing     = true;
        }
    }
    else /*bio_mode*/
    {
        while (_ox->sample_log < INTERVAL)
        {
            if (BIO_MODE == _ox->mode)
            {
                if (samples_ready)
                {
                    /*only read data if available (samples_ready is asserted by ISR and cleared by get_sensor_data) */
                    get_sensor_data(_ox, _ob, _sp);
                    _ox->samples_processed  = true;
                    _ox->sample_log         = (uint8_t)(_ox->sample_log + 10);
                }
            }

            else
            {
                /*not bio mode */
                break; //exit loop and go to sleep
            }

            /***********Run algorithm************************/

            if ((1 == _ox->do_alg) && _ox->fifo_mode && (true == _ox->samples_processed))
            {
                /*changes do_alg to 2 so it doesn't run again until all samples have been collected*/
                do_algorithm (_ox, _ob, _sp);
                do_segger_print (_ox, _sp);
                _ox->samples_processed = (uint8_t)1 - _ox->ppg2; /*if spo2 mode collect another round of samples*/
            }

            /***********Collect other samples***********************/
            if (true == _ox->samples_processed)
            {
                check_for_alg_reset (_ox, _ob, _sp);
                if (INTERVAL == _ox->sample_log)
                {
                    _ox->do_alg             = 1; /*go back and run the algorithm*/
                    _ox->samples_processed  = false;
                }
            }

            /* Wait for a new sample ready interrupt */
            if (BIO_MODE == _ox->mode)
            {
                /* Must read this register to clear interrupt state */
                i2c_r (OB1203_ADDR, REG_STATUS_0, status_reg_contents, 2);

                /* Wait for a new sample ready interrupt */
                xSemaphoreTake(g_ob1203_semaphore, portMAX_DELAY);
            }
        }//end sample collection loop
        _ox->sample_log = 0;
    } //end bio mode case
}
/**********************************************************************************************************************
 End of function ob1203_spo2_main
 *********************************************************************************************************************/


