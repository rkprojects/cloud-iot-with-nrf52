# Cloud IoT With nRF52840

This project is part of the article: [https://ravikiranb.com/articles/connecting-to-cloud/](https://ravikiranb.com/articles/connecting-to-cloud/)

Connectivity to AWS and GCP IoT Core services can be either with mbedTLS or SIM7600E's SSL AT commands. Example application serves as a starting point to build IoT applications over cellular network. As a simple task it will periodically publish SoC temperature with timestamp and additionally for GCP it will subscribe to configuration and commands topic.

# Development Board

Any nRF52840 based custom board or Nordic Semiconductor's offical [nRF52840 DK](https://www.nordicsemi.com/Software-and-Tools/Development-Kits/nRF52840-DK) will work. The project uses only the first two BSP LEDs (trivial indication) and two UARTE instances: one for debug output on usb to serial converter and second one for AT modem. For UART, only the Tx,Rx pins are used, no hardware flow control. 

As of now I have tested this project on my simple custom board based on EBYTE E73-2G4M08S1C module + pyOCD + any [CMSIS-DAP Debug Unit](https://github.com/rkprojects/openlink-v1-cmsis-dap) + Ubuntu 16.04 LTS. If you are also going to use EBYTE module then please note that it is locked by default and should be unlocked with either openocd or (nrfjprog + jlink).

# Get Source Code

git clone https://github.com/rkprojects/cloud-iot-with-nrf52.git

# Configure Project

Project builds with just a single Makefile similar to nRF52 SDK examples with armgcc. SES IDE is not used as it will be restricted to jlink.

Two boards are included in the source codes:  

* *cloud-iot-with-nrf52/boards/b840_block* - Custom board.  
* *cloud-iot-with-nrf52/boards/pca10056* - Offical DK board.  

You can use either board's Makefile to adapt to your board. You just have to change the board name in the Makefile. Refer to [SDK documentation for custom boards](https://infocenter.nordicsemi.com/topic/com.nordic.infocenter.sdk5.v15.3.0/sdk_for_custom_boards.html).

## Change Makefile

Lets assume Offical DK board from here, open its makefile: *cloud-iot-with-nrf52/boards/pca10056/blank/armgcc/Makefile*

### Set nRF5 SDK Root

Set **SDK_ROOT** variable to either absolute or relative path of nRF5 SDK. (Tested with nRF5_SDK_15.3.0_59ac345)


### Select Cloud Target

There are four possible combinations:

* CLOUD_TARGET_AWS_GPRS_SSL: AWS IoT → GRPS SSL APIs  
* CLOUD_TARGET_AWS_MBEDTLS_GPRS_TCP: AWS IoT → mbedTLS → GPRS TCP APIs  
* CLOUD_TARGET_GCP_MBEDTLS_GPRS_SSL: GCP IoT → mbedTLS (JWT only) + GRPS SSL APIs  
* CLOUD_TARGET_GCP_MBEDTLS_GPRS_TCP: GCP IoT → mbedTLS → GPRS TCP APIs  

Set **CLOUD_TARGET** variable to one of the above option.

## Configure UARTE Pins

Open header file *cloud-iot-with-nrf52/include/uarte.h*

* Change UART_MODEM_RX/TX_PIN_PSEL as per the free pins available on the board header. 
* Change UART_DEBUG_RX/TX_PIN_PSEL as per the free pins available on the board header. 

## BK-SIM7600E 4G LTE Board Connections

[BK-SIM7600E](http://and-global.com/index.php/product/SIM7600E%20CAT1.html) is a 4G LTE breakout board from AND Technologies. Any SIMCOM SIM7600E module based board would do for this project. This board's UART pins are at 3.3V levels so it can be directly connected to nRF52840. If board has been used in some other projects then do a factory reset to restore manufacturer settings. UART communication settings are 115200 bps baud rate, 8-N-1 data format.

* Connect UART_MODEM_RX_PIN -> BK-SIM7600E.TX
* Connect UART_MODEM_TX_PIN -> BK-SIM7600E.RX
* Connect DK Board's GND -> BK-SIM7600E.GND
* BK-SIM7600E.GND -> supply GND. It has two ground pins.
* BK-SIM7600E.VCC -> supply 5V

Power key feature of the module is not used. It is always kept ON, soft reset is done by the firmware.

**Note on SIM card: SIM Pin feature is not used. If Pin for your SIM card is enabled then disable it first in your phone and then insert in the module, preferably 4G SIM.**

**Note on PDP Context: By default after registration, module automatically gets PDP contexts and enables them. If your network needs a custom PDP then you will have to add it.**


## Debug Output/Console Connections

Get any USB to Serial converter like FTDI or CP210x. UART communication settings are 115200 bps baud rate, 8-N-1 data format. Converter should have 3.3V logic levels, these are usually configurable.

* Connect UART_DEBUG_RX_PIN -> USB-Serial.TX (Debug Rx is not used)
* Connect UART_DEBUG_TX_PIN -> USB-Serial.RX
* Connect DK Board's GND -> USB-Serial.GND

## Integrate SSL Certificates

By default there are no certificates integrated with source code as these are very specific
to your project. 

Certificates and any other arbitrary file types are stored in a simple read only file system as part of the code (const char array):  

* Create root directory to store files for read only file system:  
    $ cd cloud-iot-with-nrf52  
    $ mkdir rofs_root  
* Copy all the certificates in PEM or CRT/DER format in this or its sub directory. Any number
of files can be added, limited by flash size. File paths in source code begins from '/' character. Example, If a file named *aws-root-ca.pem* is copied to *cloud-iot-with-nrf52/rofs_root/certs* directory, in source code its path will be */certs/aws-root-ca.pem*
* Generate the file system:  
$ cd cloud-iot-with-nrf52/scripts  
$ python3 generate_rofs.py  


## Configure IoT Settings

Settings of AWS and GCP are configured in *cloud-iot-with-nrf52/include/aws_iot_config.h*.  
If using GCP, use their long term domain host *mqtt.2030.ltsapis.goog* and convert and concatenate primary, backup Root CA certificate into one PEM file.


# Build and Run

$ cd cloud-iot-with-nrf52/boards/pca10056/blank/armgcc  
$ make  

Build will fail if read-only filesystem is not generated or IoT settings are not configured.

$ make flash

Connect to debug console with any tty tools like Cutecom or minicom to view program output.


# Known Issues

* nRF52840 UARTE EasyDMA does not have any realtime way of indicating how much data has been transferred for circular mode use case. Currently circular data transfer progress is implemented with interrupts, but it is inefficient. A more efficient implementation with Programmable peripheral interconnect (PPI) + Timer/Counter is pending.
* For Cloud Target **CLOUD_TARGET_GCP_MBEDTLS_GPRS_SSL**, SIM7600E SSL AT Commands fails in TLS handshake stage for ECC keys when server authentication is enabled.

# License

Copyright 2019 Ravikiran Bukkasagara <contact@ravikiranb.com>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
