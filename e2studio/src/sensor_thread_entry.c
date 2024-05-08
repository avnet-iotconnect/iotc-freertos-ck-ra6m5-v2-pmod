/***********************************************************************************************************************
 * File Name    : sensor_thread_entry.c
 * Description  : Contains data structures and functions
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
 ************************************************************************************************************************
 * This code was created by Dan Allen, 2020, Renesas Electronics America. It is intended for demonstration purposes only, not
 * for production. No performance or suitability for any purpose including medical devices is guaranteed or claimed. For
 * questions and inquiries visit Renesas.com

 **********************************************************************************************************************/
#include <icp_20100.h>
#include <ra_hs3001.h>
#include "sensor_thread.h"
#include "ra_zmod4xxx_common.h"
#include "common_utils.h"
#include "user_choice.h"
#include "rmci2c.h"


void ra_init (void);
#define INT_CHANNEL (1)

uint8_t g_zmod_init_flag = 0;
uint8_t g_icp_init_flag  = 0;

/**********************************************************************************************************************
 * Function Name: reset_zmod_sensor
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static reset_zmod_sensor(void)
{
    R_BSP_PinAccessEnable();

    /* ZMOD Reset for CK-RA5M5 */
    R_BSP_PinWrite((bsp_io_port_pin_t)BSP_IO_PORT_03_PIN_06, BSP_IO_LEVEL_HIGH); // 4510
    R_BSP_PinWrite((bsp_io_port_pin_t)BSP_IO_PORT_03_PIN_07, BSP_IO_LEVEL_HIGH); // 4410
    vTaskDelay(10);

    R_BSP_PinWrite((bsp_io_port_pin_t)BSP_IO_PORT_03_PIN_06, BSP_IO_LEVEL_LOW); // 4510
    R_BSP_PinWrite((bsp_io_port_pin_t)BSP_IO_PORT_03_PIN_07, BSP_IO_LEVEL_LOW); // 4410
    vTaskDelay(10);

    R_BSP_PinWrite((bsp_io_port_pin_t)BSP_IO_PORT_03_PIN_06, BSP_IO_LEVEL_HIGH); // 4510
    R_BSP_PinWrite((bsp_io_port_pin_t)BSP_IO_PORT_03_PIN_07, BSP_IO_LEVEL_HIGH); // 4410
    vTaskDelay(10);

    R_BSP_PinAccessDisable();
}
/**********************************************************************************************************************
 End of function reset_zmod_sensor
 *********************************************************************************************************************/


/*******************************************************************************************************************//**
 * @brief       Initialization of UART and external interrupt
 * @param[in]
 * @retval
 * @retval
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: ra_init
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void ra_init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Initialising UART */
    err = uart_initialize();
    if (FSP_SUCCESS != err)
    {
        APP_PRINT ("\r\n **UART INIT FAILED ** \r\n");
        APP_ERR_TRAP (err);
    }
    else
    {
        APP_PRINT ("\r\n **UART INIT SUCCESS ** \r\n");
    }

    /* opening ExternalIRQ for IRQ14 P403 for OB1203 */
    err = R_ICU_ExternalIrqOpen (&g_sensorIRQ_ctrl, &g_sensorIRQ_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_TRAP(err);
    }
    else
    {
        APP_PRINT("** OB1203 Sensor External Irq Open success ** %u \r\n",err);
    }

}
/**********************************************************************************************************************
 End of function ra_init
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: g_comms_i2c_bus0_quick_setup
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void g_comms_i2c_bus0_quick_setup(void)
{
    fsp_err_t err;
    i2c_master_instance_t * p_driver_instance =
                (i2c_master_instance_t *) g_comms_i2c_bus0_extended_cfg.p_driver_instance;

    /* Open I2C driver, this must be done before calling any COMMS API */
    err = p_driver_instance->p_api->open(p_driver_instance->p_ctrl, p_driver_instance->p_cfg);
    if (FSP_SUCCESS != err)
    {
    APP_PRINT("I2C bus setup success:%d\n",err);
    }
    else
    {
    APP_PRINT("I2C bus setup failed:%d\n",err);
    }

    /* Create a semaphore for blocking if a semaphore is not NULL */
    if (NULL != g_comms_i2c_bus0_extended_cfg.p_blocking_semaphore)
    {
        *(g_comms_i2c_bus0_extended_cfg.p_blocking_semaphore->p_semaphore_handle)
                                        = xSemaphoreCreateCountingStatic((UBaseType_t) 1, (UBaseType_t) 0,
                                        g_comms_i2c_bus0_extended_cfg.p_blocking_semaphore->p_semaphore_memory);
    }

    /* Create a recursive mutex for bus lock if a recursive mutex is not NULL */
    if (NULL != g_comms_i2c_bus0_extended_cfg.p_bus_recursive_mutex)
    {
        *(g_comms_i2c_bus0_extended_cfg.p_bus_recursive_mutex->p_mutex_handle)
                                        = xSemaphoreCreateRecursiveMutexStatic
                                        (g_comms_i2c_bus0_extended_cfg.p_bus_recursive_mutex->p_mutex_memory);
    }
}
/**********************************************************************************************************************
 End of function g_comms_i2c_bus0_quick_setup
 *********************************************************************************************************************/





/**********************************************************************************************************************
 * Function Name: g_comms_i2c_bus1_quick_setup
 * Description  : Quick setup for g_comms_i2c_bus1
 * Return Value : .
 *********************************************************************************************************************/
void g_comms_i2c_bus1_quick_setup(void)
{
    fsp_err_t err;
    i2c_master_instance_t * p_driver_instance =
            (i2c_master_instance_t *) g_comms_i2c_bus1_extended_cfg.p_driver_instance;

    /* Open I2C driver, this must be done before calling any COMMS API */
    err = p_driver_instance->p_api->open(p_driver_instance->p_ctrl, p_driver_instance->p_cfg);
    if (FSP_SUCCESS != err)
    {
    APP_PRINT("I2C bus 1 setup success:%d\n",err);
    }
    else
    {
    APP_PRINT("I2C bus 1 setup failed:%d\n",err);
    }

    /* Create a semaphore for blocking if a semaphore is not NULL */
    if (NULL != g_comms_i2c_bus1_extended_cfg.p_blocking_semaphore)
    {
        *(g_comms_i2c_bus1_extended_cfg.p_blocking_semaphore->p_semaphore_handle)
                                        = xSemaphoreCreateCountingStatic((UBaseType_t) 1, (UBaseType_t) 0,
                                        g_comms_i2c_bus1_extended_cfg.p_blocking_semaphore->p_semaphore_memory);
    }

    /* Create a recursive mutex for bus lock if a recursive mutex is not NULL */
    if (NULL != g_comms_i2c_bus2_extended_cfg.p_bus_recursive_mutex)
    {
        *(g_comms_i2c_bus1_extended_cfg.p_bus_recursive_mutex->p_mutex_handle)
                                        = xSemaphoreCreateRecursiveMutexStatic
                                        (g_comms_i2c_bus1_extended_cfg.p_bus_recursive_mutex->p_mutex_memory);
    }
}
/**********************************************************************************************************************
 End of function g_comms_i2c_bus1_quick_setup
 *********************************************************************************************************************/






/**********************************************************************************************************************
 * Function Name: g_comms_i2c_bus2_quick_setup
 * Description  : Quick setup for g_comms_i2c_bus2
 * Return Value : .
 *********************************************************************************************************************/
void g_comms_i2c_bus2_quick_setup(void)
{
    fsp_err_t err;
    i2c_master_instance_t * p_driver_instance =
            (i2c_master_instance_t *) g_comms_i2c_bus2_extended_cfg.p_driver_instance;
    /* Open I2C driver, this must be done before calling any COMMS API */
    err = p_driver_instance->p_api->open(p_driver_instance->p_ctrl, p_driver_instance->p_cfg);
    if (FSP_SUCCESS != err)
    {
    APP_PRINT("I2C bus setup success:%d\n",err);
    }
    else
    {
    APP_PRINT("I2C bus setup failed:%d\n",err);
    }

    /* Create a semaphore for blocking if a semaphore is not NULL */
    if (NULL != g_comms_i2c_bus2_extended_cfg.p_blocking_semaphore)
    {
    *(g_comms_i2c_bus2_extended_cfg.p_blocking_semaphore->p_semaphore_handle)
                                    = xSemaphoreCreateCountingStatic((UBaseType_t) 1, (UBaseType_t) 0,
                                    g_comms_i2c_bus2_extended_cfg.p_blocking_semaphore->p_semaphore_memory);
    }

    /* Create a recursive mutex for bus lock if a recursive mutex is not NULL */
    if (NULL != g_comms_i2c_bus2_extended_cfg.p_bus_recursive_mutex)
    {
    *(g_comms_i2c_bus2_extended_cfg.p_bus_recursive_mutex->p_mutex_handle)
                                    = xSemaphoreCreateRecursiveMutexStatic
                                    (g_comms_i2c_bus2_extended_cfg.p_bus_recursive_mutex->p_mutex_memory);
    }
}
/**********************************************************************************************************************
 End of function g_comms_i2c_bus2_quick_setup
 *********************************************************************************************************************/





/**********************************************************************************************************************
 * Function Name: sensor_thread_entry
 * Description  : Sensor_Thread entry function.
 * Argument     : pvParameters contains TaskHandle_t
 * Return Value : .
 *********************************************************************************************************************/
void sensor_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    ra_init();
    g_comms_i2c_bus0_quick_setup();
    g_comms_i2c_bus1_quick_setup();
    g_comms_i2c_bus2_quick_setup();

    reset_zmod_sensor();

#if ICP20100_SENSOR_ENABLE
    RmComDevice_init();


    if (ICP20100_Sensor_init() == FSP_SUCCESS)
    {
        g_icp_init_flag = 1;
    }

    else
    {
        if (ICP20100_Sensor_init() == FSP_SUCCESS)
        {
            g_icp_init_flag = 1;
        }
    }
#endif

#if  HS3001_SENSOR_ENABLE
    /* Open HS3001 */
    g_hs300x_sensor0_quick_setup();
#endif

#if  ZMOD4410_SENSOR_ENABLE
    /* Open ZMOD4410 */
    g_zmod4xxx_sensor0_quick_setup();
#endif

    g_zmod_init_flag = 1;


#if ICM20948_SENSOR_ENABLE
    /* Open ICM20948 */
    rmcomdevice_init_icm();
    ICM20948_Sensor_init();
#endif

#if ICM42605_SENSOR_ENABLE
    /* Open ICM42605 */
    rmcomdevice_init_icm();
    icm42605_sensor_init();
#endif
    /* start user timer */

    user_timer_start();
    while (1)
    {

#if HS3001_SENSOR_ENABLE
        hs3001_get();
        vTaskDelay(100);
#endif

#if ICM42605_SENSOR_ENABLE
        icm_42605_get();
        vTaskDelay(100);
#endif

#if ICM20948_SENSOR_ENABLE
        ICM_20948_get();
        delay(300);
#endif
    }
}
/**********************************************************************************************************************
 End of function sensor_thread_entry
 *********************************************************************************************************************/

