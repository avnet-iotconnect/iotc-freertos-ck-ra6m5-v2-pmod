## About
Avnet IoTConnect AT Command Interface project example for the Renesas CK-RA6M5 v2.

This code is based on the [CK-RA6M5 v2 Sample Code](https://www.renesas.com/us/en/products/microcontrollers-microprocessors/ra-cortex-m-mcus/ck-ra6m5-cloud-kit-based-ra6m5-mcu-group#documents) project and uses an [IoTConnect AT Command-enabled PMOD module (e.g. DA16K based)](https://github.com/avnet-iotconnect/iotc-dialog-da16k-sdk) as a gateway.

`quickstart_ck_ra6m5_v2_ep` Copyright 2023 Renesas Electronics Corporation and/or its affiliates.

## Dependencies
The project uses the following dependent projects:
* [iotc-freertos-da16k-atcmd-lib](https://github.com/avnet-iotconnect/iotc-freertos-da16k-atcmd-lib)

A fully set up DA16600 PMOD Module is required to be set up and connected to IoTConnect.

Follow the [DA16K AT Interface Quick Start guide.](https://github.com/avnet-iotconnect/iotc-dialog-da16k-sdk/blob/main/doc/QUICKSTART.md)


## Supported Boards

[Renesas CK-RA6M5 v2 Cloud Kit](https://www.renesas.com/us/en/products/microcontrollers-microprocessors/ra-cortex-m-mcus/ck-ra6m5-cloud-kit-based-ra6m5-mcu-group) with [DA16600 Wi-Fi-BLE combo module](https://www.renesas.com/us/en/products/wireless-connectivity/wi-fi/low-power-wi-fi/da16600mod-ultra-low-power-wi-fi-bluetooth-low-energy-combo-modules-battery-powered-iot-devices).


## Supported IoTC Device Commands

You can add these to your device template in the IoTConnect Dashboard.

* `set_red_led <state>`

    Controls state of red LED on the board

    * `on` - Turns red LED on
    * `off` - Truns red LED off

* `set_led_frequency <freq>`

    Controls the frequency of the blue LED flashing on the board

    * `0` - Slow blinking
    * `1` - Medium blinking
    * `2` - Fast blinking
    * `3` - Very fast blinking

## Getting Started

Follow the [Renesas CK-RA6M5 v2 Quick Start Guide](https://www.renesas.com/us/en/products/microcontrollers-microprocessors/ra-cortex-m-mcus/ck-ra6m5-cloud-kit-based-ra6m5-mcu-group#documents) to set up your CK-RA6M5 v2 board for use.

**Make sure the DA16x00 device is connected to the PMOD1 connector.**

**Follow the [Quickstart Guide](./QUICKSTART.md) to flash the included binary image and use it.**

# Building & Development

* Install **Python 3.8 or higher**
* Install the [Flexible Software Package with e² Studio IDE](https://www.renesas.com/us/en/software-tool/flexible-software-package-fsp).

The project is tested and built with FSP version 5.0.0, but it may work with later 5.x.x versions.

You can now open / import, build and debug the project as per the Renesas Quick Start guide.

## Enabling Automatic IoTConnect / Device Configuration

The underlying ATCMD library can configure the device automatically using certain APIs.

**This allows you to skip manual configuration of the DA16xxx PMOD device** (you are, however, still required to flash the firmware on it.)

The file `iotc_demo_thread_entry.c` contains definitions for these:

* Define `DA16K_IOTC_CONFIG_USED` in the project to use custom IoTConnect configuration parameters
* Define `DA16K_WIFI_CONFIG_USED` in the project to use custom WiFi configuration parameters

`DA16K_IOTC_AWS` may be replaced with `DA16K_IOTC_AZURE` to fit your environment.

![](assets/cfgdef.png)

### Adding a Device Client Certificate / Private Key for Testing

**NOTE: This requires a custom IOTC Configuration via `DA16K_IOTC_CONFIG_USED` to work (see above)!**

You may place device client certificate and private key files in the PEM format into the **`e2studio/cert/`** directory.

The file names must be:

* `device_cert.pem` (client cert)
* `device_key.pem` (private key)

There is an automatic pre-build step in the form of a python script, `/util/pre_build.py`. which generates a header file from these.
