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
static inline double hs300x_data_to_float(const rm_hs300x_sensor_data_t *data) {
    assert(NULL != data);
    return ((double) data->integer_part) + ((double) (data->decimal_part) / 100.0);
}

static inline bool string_starts_with(const char *full_string, const char *to_check) {
    return strncmp(to_check, full_string, strlen(to_check)) == 0;
}

static inline bool command_has_params(const da16k_cmd_t *cmd) {
    return cmd->parameters != NULL;
}

static void iotc_demo_handle_command(const da16k_cmd_t *cmd) {
    /* All commands we know need parameters. */

    if (!command_has_params(cmd)) {
        printf_colour("ERROR: Command '%s' needs a parameter!\r\n");
        return;
    }

    if        (string_starts_with(cmd->command, "set_led_frequency")) {
        set_led_frequency((uint16_t) atoi(cmd->parameters));
        
    } else if (string_starts_with(cmd->command, "set_red_led")) {

        if        (string_starts_with(cmd->parameters, "on")) {
            TURN_RED_ON
        } else if (string_starts_with(cmd->parameters, "off")) {
            TURN_RED_OFF            
        } else {
            printf_colour("ERROR: unknown parameter for %s\r\n", cmd->command);
        }

    } else {
        printf_colour("ERROR: Unknown command received: %s\r\n", cmd->command);
        return;

    }
}

/* Custom IoTConnect configuration parameters - define DA16K_IOTC_CONFIG_USED to use them */
#if defined (DA16K_IOTC_CONFIG_USED)

#define IOTC_CONNECTION_TYPE    DA16K_IOTC_AWS
#define IOTC_CPID               "<INSERT CPID HERE>"
#define IOTC_DUID               "<INSERT DUID HERE>"
#define IOTC_ENV                "<INSERT ENV HERE>"

static da16k_wifi_cfg_t iotcConfig = { IOTC_CONNECTION_TYPE, IOTC_CPID, IOTC_DUID, IOTC_ENV, 0, NULL, NULL, NULL }
#define IOTC_CONFIG_PTR &iotcConfig
#else
#define IOTC_CONFIG_PTR NULL
#endif

/* Custom WiFi configuration parameters - define DA16K_WIFI_CONFIG_USED to use them */
#if defined (DA16K_WIFI_CONFIG_USED)
#define IOTC_SSID               "<INSERT SSID HERE>"
#define IOTC_PASSPHRASE         "<INSERT PASSPHRASE HERE>"

static da16k_wifi_cfg_t wifiConfig = { IOTC_SSID, IOTC_PASSPHRASE, false, 0 };
#define WIFI_CONFIG_PTR &wifiConfig
#else
#define WIFI_CONFIG_PTR NULL
#endif

/* IoTCDemo entry function */
/* pvParameters contains TaskHandle_t */

void iotc_demo_thread_entry(void *pvParameters) {
    FSP_PARAMETER_NOT_USED (pvParameters);

    da16k_cfg_t da16kConfig = { IOTC_CONFIG_PTR, WIFI_CONFIG_PTR, 0 };

    st_sensor_data_t prev_sensor_data = {0};
    st_sensor_data_t new_sensor_data = {0};

    da16k_err_t err = da16k_init(&da16kConfig);

    assert(err == DA16K_SUCCESS);

    while (1) {
        da16k_cmd_t current_cmd = {0};

        err = da16k_get_cmd(&current_cmd);
    
        if (current_cmd.command) {
            iotc_demo_handle_command(&current_cmd);
            da16k_destroy_cmd(current_cmd);
        }

        if (err == DA16K_SUCCESS) {
            printf_colour("Command received: %s, parameters: %s\r\n", current_cmd.command, current_cmd.parameters ? current_cmd.parameters : "<none>" );
        }

        /* obtain sensor data */

        sensor_thread_get_status(&new_sensor_data);

        /* Renesas HS3001 */

        da16k_send_msg_direct_float("hs3001_humidity",    hs300x_data_to_float(&new_sensor_data.hs300x.hs300x_data.humidity));
        da16k_send_msg_direct_float("hs3001_temperature", hs300x_data_to_float(&new_sensor_data.hs300x.hs300x_data.temperature));

        /* Renesas OB1203SD-C4R */

        da16k_send_msg_direct_bool ("ob1203_calibrated", new_sensor_data.ob1203.calibrated);

        if (new_sensor_data.ob1203.calibrated) {
            da16k_send_msg_direct_bool ("ob1203_sensing", new_sensor_data.ob1203.sensing);

            if (new_sensor_data.ob1203.sensing) {
                da16k_send_msg_direct_uint("ob1203_oxygen",    new_sensor_data.ob1203.ob_spo2);
                da16k_send_msg_direct_uint("ob1203_heartRate", new_sensor_data.ob1203.ob_hr);
            }
        }

        /* Renesas ZMOD4410 */

        da16k_send_msg_direct_bool ("zmod4410_calibrated", new_sensor_data.zmod4410.calibrated);

        if (new_sensor_data.zmod4410.calibrated) {
            da16k_send_msg_direct_float("zmod4410_airQualityIndex",                new_sensor_data.zmod4410.zmod4410Data.iaq);
            da16k_send_msg_direct_float("zmod4410_carbonDioxideLevel",             new_sensor_data.zmod4410.zmod4410Data.ec02);
            da16k_send_msg_direct_float("zmod4410_totalVolatileOrganicCompounds",  new_sensor_data.zmod4410.zmod4410Data.tvoc);
        }

        /* TDK ICM-42605 */

        da16k_send_msg_direct_bool ("icm42605_available", new_sensor_data.icm42605.available);

        if (new_sensor_data.icm42605.available) {
            da16k_send_msg_direct_float("icm42605_gyroX",  new_sensor_data.icm42605.my_gyro.x);
            da16k_send_msg_direct_float("icm42605_gyroY",  new_sensor_data.icm42605.my_gyro.x);
            da16k_send_msg_direct_float("icm42605_gyroZ",  new_sensor_data.icm42605.my_gyro.x);
            da16k_send_msg_direct_float("icm42605_accelX", new_sensor_data.icm42605.my_accel.x);
            da16k_send_msg_direct_float("icm42605_accelY", new_sensor_data.icm42605.my_accel.x);
            da16k_send_msg_direct_float("icm42605_accelZ", new_sensor_data.icm42605.my_accel.x);
        }

        /* TDK ICP-20100 */

        da16k_send_msg_direct_float("icp20100_temperature", new_sensor_data.icp20100.temperatureicp);
        da16k_send_msg_direct_float("icp20100_pressure",    new_sensor_data.icp20100.pressureicp);

        memcpy(&prev_sensor_data, &new_sensor_data, sizeof(st_sensor_data_t));

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
