#include <iotc_demo_thread.h>
#include "da16k_comm/da16k_comm.h"
#include "common_init.h"
#include "iotc_demo.h"
#include "user_board_cfg.h"

/* Telemetry grabber code
 *
 * Obtains sensor data from st_sensor_data_t and massages it to key/value pairs.
 *
 */

extern void sensor_thread_get_status(st_sensor_data_t *p_data);

/* HS300X sensor data has a weird format where the decimal part is separate and goes from 00-99
 * so we need to massage it a little bit ...*/
static inline float hs300xDataToFloat(const rm_hs300x_sensor_data_t *data) {
    assert(NULL != data);
    return ((float) data->integer_part) + ((float) (data->decimal_part) / 100.0f);
}

static inline bool stringStartsWith(const char *fullString, const char *toCheck) {
    return strncmp(toCheck, fullString, strlen(toCheck)) == 0;
}

static inline bool commandHasParams(const da16k_cmd_t *cmd) {
    return cmd->parameters != NULL;
}

static void iotcDemoHandleCommand(const da16k_cmd_t *cmd) {
    /* All commands we know need parameters. */

    if (!commandHasParams(cmd)) {
        printf_colour("ERROR: Command '%s' needs a parameter!\r\n");
        return;
    }

    if        (stringStartsWith(cmd->command, "set_led_frequency")) {
        set_led_frequency((uint16_t) atoi(cmd->parameters));
        
    } else if (stringStartsWith(cmd->command, "set_red_led")) {

        if        (stringStartsWith(cmd->parameters, "on")) {
            TURN_RED_ON
        } else if (stringStartsWith(cmd->parameters, "off")) {
            TURN_RED_OFF            
        } else {
            printf_colour("ERROR: unknown parameter for %s\r\n", cmd->command);
        }

    } else {
        printf_colour("ERROR: Unknown command received: %s\r\n", cmd->command);
        return;

    }
}


/* IoTCDemo entry function */
/* pvParameters contains TaskHandle_t */

void iotc_demo_thread_entry(void *pvParameters) {
    FSP_PARAMETER_NOT_USED (pvParameters);

    da16k_cfg_t da16kConfig;

    st_sensor_data_t previousSensorData = {0};
    st_sensor_data_t newSensorData = {0};

    da16k_err_t err = da16k_init(&da16kConfig);

    assert(err == DA16K_SUCCESS);

    while (1) {
        da16k_cmd_t currentCmd = {0};

        err = da16k_getCmd(&currentCmd);
    
        if (currentCmd.command) {
            iotcDemoHandleCommand(&currentCmd);
        }

        if (err == DA16K_SUCCESS) {
            printf_colour("Command received: %s, parameters: %s\r\n", currentCmd.command, currentCmd.parameters ? currentCmd.parameters : "<none>" );
        }
        
        /* obtain sensor data */

        sensor_thread_get_status(&newSensorData);

        /* Renesas HS3001 */

        da16k_send_float("hs3001_humidity",    hs300xDataToFloat(&newSensorData.hs300x.hs300x_data.humidity));
        da16k_send_float("hs3001_temperature", hs300xDataToFloat(&newSensorData.hs300x.hs300x_data.temperature));

        /* Renesas OB1203SD-C4R */

        da16k_send_bool ("ob1203_calibrated", newSensorData.ob1203.calibrated);

        if (newSensorData.ob1203.calibrated) {
            da16k_send_bool ("ob1203_sensing", newSensorData.ob1203.sensing);

            if (newSensorData.ob1203.sensing) {
                da16k_send_uint("ob1203_oxygen",    newSensorData.ob1203.ob_spo2);
                da16k_send_uint("ob1203_heartRate", newSensorData.ob1203.ob_hr);
            }
        }

        /* Renesas ZMOD4410 */

        da16k_send_bool ("zmod4410_calibrated", newSensorData.zmod4410.calibrated);

        if (newSensorData.zmod4410.calibrated) {
            da16k_send_float("zmod4410_airQualityIndex",                newSensorData.zmod4410.zmod4410Data.iaq);
            da16k_send_float("zmod4410_carbonDioxideLevel",             newSensorData.zmod4410.zmod4410Data.ec02);
            da16k_send_float("zmod4410_totalVolatileOrganicCompounds",  newSensorData.zmod4410.zmod4410Data.tvoc);
        }

        /* TDK ICM-42605 */

        da16k_send_bool ("icm42605_available", newSensorData.icm42605.available);

        if (newSensorData.icm42605.available) {
            da16k_send_float("icm42605_gyroX",  newSensorData.icm42605.my_gyro.x);
            da16k_send_float("icm42605_gyroY",  newSensorData.icm42605.my_gyro.x);
            da16k_send_float("icm42605_gyroZ",  newSensorData.icm42605.my_gyro.x);
            da16k_send_float("icm42605_accelX", newSensorData.icm42605.my_accel.x);
            da16k_send_float("icm42605_accelY", newSensorData.icm42605.my_accel.x);
            da16k_send_float("icm42605_accelZ", newSensorData.icm42605.my_accel.x);
        }

        /* TDK ICP-20100 */

        da16k_send_float("icp20100_temperature", newSensorData.icp20100.temperatureicp);
        da16k_send_float("icp20100_pressure",    newSensorData.icp20100.pressureicp);

        memcpy(&previousSensorData, &newSensorData, sizeof(st_sensor_data_t));

    }
}
