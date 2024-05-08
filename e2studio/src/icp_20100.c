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
 * File Name    : icp_20100.c
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

/*
 * icp_20100.c
 *
 *  Created on: 10-Mar-2023
 *      Author: 31342
 */

#include <icp_20100.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "rmci2c.h"
#include "r_typedefs.h"

typedef enum e_demo_callback_status
{
    DEMO_CALLBACK_STATUS_WAIT = (0),
    DEMO_CALLBACK_STATUS_SUCCESS,
    DEMO_CALLBACK_STATUS_REPEAT,
} demo_callback_status_t;

demo_callback_status_t  g_irq6_callback_status;

volatile float32_t Pressure;
volatile float32_t Temperature;
volatile float32_t ChPressure;
extern uint8_t g_icp_init_flag;

uint8_t fifo_fill_reg_contents  = 0U;
uint8_t int_reg_contents        = 0U;

/**********************************************************************************************************************
 * Function Name: ICP20100_Sensor_init
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
int8_t ICP20100_Sensor_init(void)
{
    int8_t err = FSP_SUCCESS;
    if (pressure20_default_cfg() == ICP_I2C_ERROR)
    {
        err = ICP_I2C_ERROR;
    }
    return err;
}
/**********************************************************************************************************************
 End of function ICP20100_Sensor_init
 *********************************************************************************************************************/




/**********************************************************************************************************************
 * Function Name: icp_20100_get
 * Description  : .
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
void icp_20100_get()
{
    int8_t err = FSP_SUCCESS;
    uint8_t flush_bit = 0U;

    if (g_icp_init_flag)
    {
        err = pressure20_read_register (PRESSURE20_REG_FIFO_FILL, &fifo_fill_reg_contents, 1);
        if ((fifo_fill_reg_contents & PRESSURE20_FIFO_LEVEL_FULL) == PRESSURE20_FIFO_LEVEL_FULL)
        {
            err = pressure20_clear_interrupts();
            pressure20_read_data (&Pressure, &Temperature);
            flush_bit = PRESSURE20_FIFO_FLUSH;

            err = pressure20_write_register(PRESSURE20_REG_FIFO_FILL, &flush_bit, 1);
        }
    }
}
/**********************************************************************************************************************
 End of function icp_20100_get
 *********************************************************************************************************************/




/**********************************************************************************************************************
 * Function Name: pressure20_default_cfg
 * Description  : .
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
int8_t pressure20_default_cfg()
{
    volatile int8_t     err;
    volatile uint8_t    reg_data;
    volatile uint8_t    offset;
    volatile uint8_t    gain;
    volatile uint8_t    hfosc;
    volatile uint8_t    data            = 0;
    volatile uint8_t    device_version  = 0U;

    /* dummy write */
    data = DUMMY;

    err = pressure20_write_register(PRESSURE20_REG_DUMMY, &data, 1);
    if (FSP_SUCCESS != err)
    {
        return ICP_I2C_ERROR;
    }

    R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);   //update

    if (ICP_I2C_ERROR == pressure20_check_communication())
    {
        return ICP_I2C_ERROR;
    }

    err = pressure20_read_register(PRESSURE20_REG_VERSION, &device_version, 1);

    if (FSP_SUCCESS != err)
    {
        return ICP_I2C_ERROR;
    }

    /* Move to stand by mode */
    err = pressure20_write_mode_select(PRESSURE20_POWER_MODE_NORMAL);
    if (FSP_SUCCESS != err)
    {
        return ICP_I2C_ERROR;
    }
    R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);

    /* Flush the FIFO to empty it */
    data = PRESSURE20_FIFO_FLUSH;

    err = pressure20_write_register(PRESSURE20_REG_FIFO_FILL, &data, 1);
    if (FSP_SUCCESS != err)
    {
        return ICP_I2C_ERROR;
    }

    /* Reset FIFO config */
    data = PRESSURE20_FIFO_LEVEL_EMPTY;

    err = pressure20_write_register(PRESSURE20_REG_FIFO_CONFIG, &data, 1);
    if (FSP_SUCCESS != err)
    {
        return ICP_I2C_ERROR;
    }

    /* Mask all interrupts */
    data = PRESSURE20_INT_MASK_ALL;

    err = pressure20_write_register(PRESSURE20_REG_INTERRUPT_MASK, &data, 1);
    if (FSP_SUCCESS != err)
    {
        return ICP_I2C_ERROR;
    }

    /* Clear interrupts */
    err = pressure20_clear_interrupts();
    if (FSP_SUCCESS != err)
    {
        return ICP_I2C_ERROR;
    }

    /* Read boot up status and avoid re-running boot up sequence if it is already done */
    err = pressure20_read_register(PRESSURE20_REG_OTP_STATUS2, &reg_data, 1);
    if (FSP_SUCCESS != err)
    {
        return ICP_I2C_ERROR;
    }
    if ((PRESSURE20_BOOT_UP_STATUS != (reg_data & PRESSURE20_BOOT_UP_STATUS)) &&
            (0xB2 != device_version))
    {
        /* Continue with boot up sequence for A1 */
        /* Set to power mode to activate the OTP power domain and get access to the main registers */
        err = pressure20_write_mode_select(PRESSURE20_POWER_MODE_ACTIVE);
        if (FSP_SUCCESS != err)
        {
            return ICP_I2C_ERROR;
        }
        R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);

        /* Unlock the main registers */
        data = PRESSURE20_MASTER_UNLOCK;

        err = pressure20_write_register(PRESSURE20_REG_MASTER_LOCK, &data, 1);
        if (FSP_SUCCESS != err)
        {
            return ICP_I2C_ERROR;
        }

        /* Enable the OTP and the write switch */
        data = (PRESSURE20_OTP_WRITE_SWITCH | PRESSURE20_OTP_ENABLE);

        err = pressure20_write_register(PRESSURE20_REG_OTP_CONFIG1, &data, 1);
        if (FSP_SUCCESS != err)
        {
            return ICP_I2C_ERROR;
        }
        R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MICROSECONDS);

        /* Toggle the OTP reset pin */
        data = PRESSURE20_OTP_RESET_SET;

        err = pressure20_write_register(PRESSURE20_REG_OTP_DBG2, &data, 1);
        if (FSP_SUCCESS != err)
        {
            return ICP_I2C_ERROR;
        }
        R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MICROSECONDS);

        data = PRESSURE20_OTP_RESET_CLEAR;

        err = pressure20_write_register(PRESSURE20_REG_OTP_DBG2, &data, 1);
        if (FSP_SUCCESS != err)
        {
            return ICP_I2C_ERROR;
        }
        R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MICROSECONDS);

        /* Program redundant read */
        data = PRESSURE20_OTP_MRA_LSB;

        err = pressure20_write_register(PRESSURE20_REG_OTP_MRA_LSB, &data, 1);
        if (FSP_SUCCESS != err)
        {
            return ICP_I2C_ERROR;
        }

        data = PRESSURE20_OTP_MRA_MSB;

        err = pressure20_write_register(PRESSURE20_REG_OTP_MRA_MSB, &data, 1);
        if (FSP_SUCCESS != err)
        {
            return ICP_I2C_ERROR;
        }

        data = PRESSURE20_OTP_MRB_LSB;

        err = pressure20_write_register(PRESSURE20_REG_OTP_MRB_LSB, &data, 1);
        if (FSP_SUCCESS != err)
        {
            return ICP_I2C_ERROR;
        }

        data = PRESSURE20_OTP_MRB_MSB;

        err = pressure20_write_register(PRESSURE20_REG_OTP_MRB_MSB, &data, 1);
        if (FSP_SUCCESS != err)
        {
            return ICP_I2C_ERROR;
        }

        data = PRESSURE20_OTP_MR_LSB;

        err = pressure20_write_register(PRESSURE20_REG_OTP_MR_LSB, &data, 1);
        if (FSP_SUCCESS != err)
        {
            return ICP_I2C_ERROR;
        }

        data = PRESSURE20_OTP_MR_MSB;

        err = pressure20_write_register(PRESSURE20_REG_OTP_MR_MSB, &data, 1);
        if (FSP_SUCCESS != err)
        {
            return ICP_I2C_ERROR;
        }


        /* Read offset, gain and hfosc */

        err = pressure20_read_otp_data(PRESSURE20_OTP_ADDRESS_OFFSET, &offset);
        if (FSP_SUCCESS != err)
        {
            return ICP_I2C_ERROR;
        }
        err = pressure20_read_otp_data(PRESSURE20_OTP_ADDRESS_GAIN, &gain);
        if (FSP_SUCCESS != err)
        {
            return ICP_I2C_ERROR;
        }

        err = pressure20_read_otp_data(PRESSURE20_OTP_ADDRESS_HFOSC, &hfosc);
        if (FSP_SUCCESS != err)
        {
            return ICP_I2C_ERROR;
        }

        /* Disable OTP and write switch */
        data = PRESSURE20_OTP_DISABLE;

        err = pressure20_write_register(PRESSURE20_REG_OTP_CONFIG1, &data, 1);
        if (FSP_SUCCESS != err)
        {
            return ICP_I2C_ERROR;
        }

        /* Write OTP values to main registers */
        data = (offset & PRESSURE20_TRIM1_MSB_PEFE_OFFSET_MASK);

        err = pressure20_write_register(PRESSURE20_REG_TRIM1_MSB, &data, 1);
        if (FSP_SUCCESS != err)
        {
            return ICP_I2C_ERROR;
        }

        err = pressure20_read_register (PRESSURE20_REG_TRIM2_MSB, &reg_data, 1);
        if (FSP_SUCCESS != err)
        {
            return ICP_I2C_ERROR;
        }

        reg_data = (reg_data & (~PRESSURE20_TRIM2_MSB_PEFE_GAIN_MASK)) | ((gain << 4) &
                PRESSURE20_TRIM2_MSB_PEFE_GAIN_MASK);
        err = pressure20_write_register (PRESSURE20_REG_TRIM2_MSB, &reg_data, 1);
        if (FSP_SUCCESS != err)
        {
            return ICP_I2C_ERROR;
        }

        data = (hfosc & PRESSURE20_TRIM2_LSB_HFOSC_MASK);

        err = pressure20_write_register (PRESSURE20_REG_TRIM2_LSB, &data, 1);
        if (FSP_SUCCESS != err)
        {
            return ICP_I2C_ERROR;
        }

        /* Lock the main register */
        data = PRESSURE20_MASTER_LOCK;

        err = pressure20_write_register (PRESSURE20_REG_MASTER_LOCK, &data, 1);
        if (FSP_SUCCESS != err)
        {
            return ICP_I2C_ERROR;
        }

        /* Move to stand by mode */
        err = pressure20_write_mode_select (PRESSURE20_POWER_MODE_NORMAL);
        if (FSP_SUCCESS != err)
        {
            return ICP_I2C_ERROR;
        }
        R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);

        /* Update boot up status to 1 */
        data    = PRESSURE20_BOOT_UP_STATUS;
        err     = pressure20_write_register (PRESSURE20_REG_OTP_STATUS2, &data, 1);
        if (FSP_SUCCESS != err)
        {
            return ICP_I2C_ERROR;
        }
    }

    /* Enable continuous measurement and set ODR to 25 Hz */
    err = pressure20_write_mode_select (PRESSURE20_MEAS_MODE_CONTINUOUS | PRESSURE20_MEAS_CONFIG_MODE0_ODR_25HZ);
    if (FSP_SUCCESS != err)
    {
        return ICP_I2C_ERROR;
    }
    R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);

    /* Flush the FIFO to empty it */
    data    = PRESSURE20_FIFO_FLUSH;
    err     = pressure20_write_register (PRESSURE20_REG_FIFO_FILL, &data, 1);
    if (FSP_SUCCESS != err)
    {
        return ICP_I2C_ERROR;
    }

    /* Wait for the FIFO full indication */
    do
    {
        R_BSP_SoftwareDelay(100, BSP_DELAY_UNITS_MILLISECONDS);
        err = pressure20_read_register (PRESSURE20_REG_FIFO_FILL, &reg_data, 1);
    } while (PRESSURE20_FIFO_FULL != (reg_data & PRESSURE20_FIFO_FULL));
    if (FSP_SUCCESS != err)
    {
        return ICP_I2C_ERROR;
    }

    /* Flush the FIFO to empty it, because of the FIR filter settling effect (warm up) on the first FIFO output */
    data    = PRESSURE20_FIFO_FLUSH;
    err     = pressure20_write_register (PRESSURE20_REG_FIFO_FILL, &data, 1);
    if (FSP_SUCCESS != err)
    {
        return ICP_I2C_ERROR;
    }

    /* Set FIFO watermark HIGH level to 1 */
    data    =   (PRESSURE20_FIFO_LEVEL_8 << 4);       //PRESSURE20_FIFO_LEVEL_1

    err     =   pressure20_write_register (PRESSURE20_REG_FIFO_CONFIG, &data, 1);
    if (FSP_SUCCESS != err)
    {
        return ICP_I2C_ERROR;
    }

    /* Unmask FIFO watermark HIGH interrupt */
    data = ~(PRESSURE20_INT_MASK_FIFO_WMK_HIGH | PRESSURE20_INT_MASK_FIFO_OVERFLOW);

    err = pressure20_write_register (PRESSURE20_REG_INTERRUPT_MASK, &data, 1);
    if (FSP_SUCCESS != err)
    {
        return ICP_I2C_ERROR;
    }

    err = pressure20_read_register (PRESSURE20_REG_INTERRUPT_MASK, &reg_data, 1);
    if (FSP_SUCCESS != err)
    {
        return ICP_I2C_ERROR;
    }

    /* Clear interrupts */
    err = pressure20_clear_interrupts();
    if (FSP_SUCCESS != err)
    {
        return ICP_I2C_ERROR;
    }

    return err;
}
/**********************************************************************************************************************
 End of function pressure20_default_cfg
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: pressure20_write_register
 * Description  : .
 * Arguments    : reg
 *              : data_in
 *              : len
 * Return Value : .
 *********************************************************************************************************************/
int8_t pressure20_write_register(uint8_t reg, uint8_t *data_in, uint32_t len)
{
    int8_t err;

    err = icp_rmcom_i2c_w(reg, data_in, len);

    return err;
}
/**********************************************************************************************************************
 End of function pressure20_write_register
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: pressure20_read_register
 * Description  : .
 * Arguments    : reg
 *              : data_out
 *              : len
 * Return Value : .
 *********************************************************************************************************************/
int8_t pressure20_read_register(uint8_t reg, uint8_t *data_out, uint32_t len)
{
    int8_t err;

    err = icp_rmcom_i2c_r(reg, data_out, len);

    return err;
}
/**********************************************************************************************************************
 End of function pressure20_read_register
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: pressure20_read_register_data
 * Description  : .
 * Arguments    : reg
 *              : data_out
 *              : len
 * Return Value : .
 *********************************************************************************************************************/
int8_t pressure20_read_register_data(uint8_t reg, uint8_t *data_out, uint32_t len)
{
    int8_t err;

    err = icp_rmcom_i2c_r(reg, data_out, len);

    return err;
}
/**********************************************************************************************************************
 End of function pressure20_read_register_data
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: pressure20_check_communication
 * Description  : .
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
int8_t pressure20_check_communication()
{
    volatile int8_t err = -1;
    volatile uint8_t device_id;

    while ((-1) == err)
    {
        if (pressure20_read_register (PRESSURE20_REG_DEVICE_ID, &device_id, 1) == FSP_SUCCESS)
        {
            if (PRESSURE20_DEVICE_ID == device_id)
            {
                err = FSP_SUCCESS;
            }
        }

        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);
    }
    return err;
}
/**********************************************************************************************************************
 End of function pressure20_check_communication
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: pressure20_write_mode_select
 * Description  : .
 * Argument     : mode_select
 * Return Value : .
 *********************************************************************************************************************/
int8_t pressure20_write_mode_select(uint8_t mode_select)
{
    int8_t err;
    uint8_t dev_status;
    do
    {
        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);
        err = pressure20_read_register(PRESSURE20_REG_DEVICE_STATUS, &dev_status, 1);
    } while (PRESSURE20_MODE_SYNC_STATUS != (dev_status & PRESSURE20_MODE_SYNC_STATUS));

    if (FSP_SUCCESS != err)
    {
        return ICP_I2C_ERROR;
    }

    err = pressure20_write_register(PRESSURE20_REG_MODE_SELECT, &mode_select, 1);
    return err;
}
/**********************************************************************************************************************
 End of function pressure20_write_mode_select
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: pressure20_clear_interrupts
 * Description  : .
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
int8_t pressure20_clear_interrupts()
{
    volatile uint8_t int_status;
    volatile int8_t err;

    err = pressure20_read_register(PRESSURE20_REG_INTERRUPT_STATUS, &int_status, 1);

    if (FSP_SUCCESS == err)
    {
        err = pressure20_write_register(PRESSURE20_REG_INTERRUPT_STATUS, &int_status, 1);
    }

    return err;
}
/**********************************************************************************************************************
 End of function pressure20_clear_interrupts
 *********************************************************************************************************************/




/**********************************************************************************************************************
 * Function Name: pressure20_read_otp_data
 * Description  : .
 * Arguments    : otp_address
 *              : data_out
 * Return Value : .
 *********************************************************************************************************************/
int8_t pressure20_read_otp_data (uint8_t otp_address, uint8_t *data_out)
{
    int8_t err;
    uint8_t otp_status;
    uint8_t data = 0;

    err = pressure20_write_register(PRESSURE20_REG_OTP_ADDRESS_REG, &otp_address, 1);
    if (FSP_SUCCESS != err)
    {
        return ICP_I2C_ERROR;
    }

    data = PRESSURE20_OTP_COMMAND_READ_ACTION;
    err = pressure20_write_register(PRESSURE20_REG_OTP_COMMAND_REG, &data, 1);
    if (FSP_SUCCESS != err)
    {
        return ICP_I2C_ERROR;
    }

    /* Wait for the OTP read to finish */
    do
    {
        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);
        err = pressure20_read_register(PRESSURE20_REG_OTP_STATUS, &otp_status, 1);
    } while (PRESSURE20_BUSY == otp_status);

    if (FSP_SUCCESS != err)
    {
        return ICP_I2C_ERROR;
    }

    /* Read the data from register */
    err = pressure20_read_register(PRESSURE20_REG_OTP_RDATA, data_out, 1);

    return err;
}
/**********************************************************************************************************************
 End of function pressure20_read_otp_data
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: pressure20_read_data
 * Description  : .
 * Arguments    : pressure
 *              : temperature
 * Return Value : .
 *********************************************************************************************************************/
int8_t pressure20_read_data(float32_t *pressure, float32_t *temperature)
{
    uint8_t data_buf[96] = { 0 };
    int8_t err;
    int32_t raw_data;
    uint8_t data = 0U;

    err = pressure20_read_register_data(PRESSURE20_REG_PRESS_DATA_0, data_buf, 96);
    if (FSP_SUCCESS != err)
    {
        return ICP_I2C_ERROR;
    }

    raw_data        =   ((int32_t) (((int32_t) (data_buf[2] & 0x0F) << 16) |
                        ((uint16_t) data_buf[1] << 8) | data_buf[0]) << 12) >> 12;

    *pressure       =   (((float32_t) raw_data / PRESSURE20_PRESSURE_RES_RAW) * PRESSURE20_PRESSURE_RES_MBAR) +
                        PRESSURE20_PRESSURE_OFFSET_MBAR;

    raw_data        =   ((int32_t) (((int32_t) (data_buf[5] & 0x0F) << 16) |
                        ((uint16_t) data_buf[4] << 8) | data_buf[3]) << 12) >> 12;

    *temperature    =   (((float32_t) raw_data / PRESSURE20_TEMPERATURE_RES_RAW) * PRESSURE20_TEMPERATURE_RES_C) +
                        PRESSURE20_TEMPERATURE_OFFSET_C;

    return err;
}
/**********************************************************************************************************************
 End of function pressure20_read_data
 *********************************************************************************************************************/




