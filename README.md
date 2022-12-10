# OpenThread RADAR Presence Detection Sensor

## Overview
This is a presence sensor that uses 60GHz [RADAR](https://www.acconeer.com/products/) pulses to detect human presence indoors. In particular, this sensor is designed to detect micro movements and perform 'filtering' by estimating distance/speed of target. This makes it suitable for applications where other technologies such as PIR/ToF sensors may be prone to inaccuracy. 

In this project, the sensor communicates over the OpenThread protocol, utilizing existing IPv6 infrastructure (without the need for a IoT protocol "translator"). In order to facilitate low-power operation, CoAP is used for the sensor to notify a server of state changes.

## Design

