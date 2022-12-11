# OpenThread Radar Presence Detection Sensor (IPR)

## Overview
This is a presence sensor that uses 60GHz [radar](https://www.acconeer.com/products/) pulses to detect human presence indoors.
In particular, this sensor is designed to detect micro movements and perform 'filtering' by estimating distance/speed of target. This makes it suitable for applications where other technologies such as PIR/ToF sensors may be prone to inaccuracy, such as detecting human presence while idling (i.e. sitting down).

In this project, the sensor communicates over the OpenThread protocol, utilizing existing IPv6 infrastructure (without the need for a IoT protocol "translator"). In order to facilitate low-power operation, CoAP is used for the sensor to notify a server of state changes.

## Hardware Design
The IPR is designed as a custom 6-layer ENIG PCB. The components run off a single 1.8v supply, due to limitations of the radar IO voltage, in order to avoid a dual-supply, level-translator situation. The main controller on the board is of the [EFR32MG24 Series](https://www.silabs.com/wireless/zigbee/efr32mg24-series-2-socs). Due to chip shortages, more than one part in this series was used with little effect on the actual functionality.
![IPR](https://github.com/edward62740/ot-IPR/blob/master/Documentation/ipr.png "IPR")

## Detection Principles
In general, the IPR uses the following software layers to process the radar data into useful detection information:

| SW Layers             | Type                | Purpose                                        |
| ----------------------|:-------------------:|:----------------------------------------------:|
| Application Algorithm | Custom              | Filters presence algo and manages power usage  |
| Presence Algorithm    | RSS (Proprietary)   | Provides useful interpretation of service data |
| Sparse Service        | RSS (Proprietary)   | Pre-processes sensor data                      |
| HAL                   | Platform            | Provide a generic HW interface to the RSS      |

## Details of Detection Algorithms
Documentation on RSS can be found [here](https://docs.acconeer.com/en/latest/exploration_tool/algo/a111/presence_detection_sparse.html).

For the application layer, the main constraints are a compromise between power consumption and sensor performance/responsiveness.
Hence, the application will adjust the sampling rate of the sensor based on the presence or absence of a target.<br>
More specifically, the sensor will sample at a rate of approx. 0.4 frames/s (radio gets priority since this is not time-critical), and based on the results of this detection, the frame rate can be increased up to 2Hz. Note that each frame contains 16 consecutive sweeps of the detection range. The application also introduces a hystersis-like behavior to the detection state.

## Communication
The IPR utilizes CoAP for low-power communication with a remote server. In this project, the server runs on the same hardware as the border router.
| Server (OTBR)         |                      | Client (IPR)       | Message                                        |
| ----------------------|:-------------------:|:-------------------:|:----------------------------------------------:|
|         GET           | ------------------> |                     | Informs client of the server IP and uri        |
|                       | <------------------ |        ACK          | Ack (must be received or will not proceed)     |
|           .           |         .           |          .          |                       .                        |
|           .           |         .           |          .          |                       .                        |
|                       | <------------------ |        PUT          | If change of detection state AND periodically  |
|         ACK           | ------------------> |                     | Ack                                            |
|           .           |         .           |          .          |                       .                        |



