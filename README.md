# OpenThread Radar Presence Detection Sensor (IPR)

## Overview
This is a presence sensor that uses 60GHz [radar](https://www.acconeer.com/products/) pulses to detect human presence indoors.<br>
The sensor communicates over the OpenThread protocol, utilizing existing IPv6 infrastructure (without the need for a IoT protocol "translator"). In order to facilitate low-power operation, CoAP is used for the sensor to notify a server of state changes.<br>
Refer here for the [CoAP Server](https://github.com/edward62740/ot-coap-server).<br>


## Hardware Design
The IPR is designed on a 6-layer, impedance controlled PCB. The components run off a single 1.8v supply, due to limitations of the radar IO voltage, in order to avoid a dual-supply, level-translator situation. The main controller on the board is of the [EFR32MG24 Series](https://www.silabs.com/wireless/zigbee/efr32mg24-series-2-socs). Due to chip shortages, more than one part in this series was used with little effect on the actual functionality.<br>
<p align="center">
<img src="https://github.com/edward62740/ot-IPR/blob/master/Documentation/ipr.png" width="800" /><br>
 <br>
  <em>IPR v2 PCB</em>
</p><br>
<p align="center">
  <img src="https://github.com/edward62740/ot-IPR/blob/master/Documentation/enclosure.png" width="350" />
  <img src="https://github.com/edward62740/ot-IPR/blob/master/Documentation/IPR%20v2.png" width="350" />
  <br>
  <em>3D printed enclosure and CAD drawings</em>
</p>


## Detection Principles
Radar was chosen for this project due to its ability to detect micro movements and perform 'filtering' by estimating distance/speed of target. This makes it suitable for certain applications where traditional sensing methods such as PIRs may be insufficient, such as needing to sense presence in specific areas, or in areas of high traffic.<br><br>
For example, consider an open space with multiple lighting zones, such as a public library or a large living/dining room. The constant motion of people walking by will need to be filtered to only activate when someone sits down or stays in a particular zone. This automatically rules out the PIR (which would be far superior in terms of power usage). <br><br>
Thus, radar appears to be an obvious choice, as well as some other solutions such as [VL53L5X](https://www.st.com/resource/en/datasheet/vl53l5cx.pdf) and [CH201](https://invensense.wpenginepowered.com/wp-content/uploads/2022/04/DS-000379-CH201-v1.2.pdf), which appear to consume similar amounts of power for this application.<br>
The obvious downside of radar over other similar technologies such as ToF, IR, etc. is that it is computationally expensive, as described below.<br><br>

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
More specifically, the sensor will sample at a rate of approx. 0.33 frames/s (radio gets priority since this is not time-critical), and based on the results of this detection, the frame rate can be increased up to 2Hz. Note that each frame contains 63 consecutive sweeps of the detection range. The application also introduces a hystersis-like behavior to the detection state. The table below shows the parameters used during testing.
<br>
Hence, the maximum time (excluding radio subsystem yields) required to switch to a "detected" state is $$\sum_{\substack{k=1 \ k \text{ odd}}}^{\left\lfloor \frac{\text{TH}{+}}{10} \right\rfloor} \max\left(\frac{\text{IFD}}{k}, FSP_{\min}\right), \quad \text{TH}_{+} \in [20,630] \cap \mathbb{Z}$$  each value is clipped by the minimum frame spacing (FSP<sub>MIN</sub>) as a lower bound. Using the configuration below, this value is 5100ms. In practice, the state often switches within 3s, as the first frame is not necessarily sampled 3000ms after presence begins.<br>
Through limited testing, this configuration yielded a false positive (i.e spurious detections without any apparent presence) rate of $4.96 \times 10^{-6}$, which is sufficient for the purposes of this project. It is yet to be determined what the false negative rate is, but the number is also trivial.

<table style="width: 145px;">
<tbody>
<tr style="height: 23px;">
<td style="width: 105.891px; height: 23px;"><strong>&nbsp;Parameter</strong></td>
<td style="width: 19.1094px; height: 23px;"><strong>Value&nbsp;</strong></td>
</tr>
<tr style="height: 23px;">
<td style="width: 105.891px; height: 23px;">Start/End Distance</td>
<td style="width: 19.1094px; height: 23px;">0.2/1.75m</td>
</tr>
<tr style="height: 23px;">
<td style="width: 105.891px; height: 23px;">Threshold (TH+/TH-/THMAX)</td>
<td style="width: 19.1094px; height: 23px;">80/20/100</td>
</tr>
<tr style="height: 23px;">
<td style="width: 105.891px; height: 23px;">dTH/dt(+/-)</td>
<td style="width: 19.1094px; height: 23px;">20/5</td>
</tr>
<tr style="height: 23.5px;">
<td style="width: 105.891px; height: 23.5px;">Rx Gain</td>
<td style="width: 19.1094px; height: 23.5px;">0.5&nbsp;</td>
</tr>
<tr style="height: 23px;">
<td style="width: 105.891px; height: 23px;">HWAAS</td>
<td style="width: 19.1094px; height: 23px;">63&nbsp;</td>
</tr>
<tr style="height: 23px;">
<td style="width: 105.891px; height: 23px;">Power Mode</td>
<td style="width: 19.1094px; height: 23px;">Off</td>
</tr>
<tr style="height: 23px;">
<td style="width: 105.891px; height: 23px;">Detection Threshold</td>
<td style="width: 19.1094px; height: 23px;">2000&nbsp;</td>
</tr>
<tr style="height: 23px;">
<td style="width: 105.891px; height: 23px;">PCA Noise Reduction</td>
<td style="width: 19.1094px; height: 23px;">1</td>
</tr>
<tr style="height: 23px;">
<td style="width: 105.891px; height: 23px;">Service Profile</td>
<td style="width: 19.1094px; height: 23px;">4&nbsp;</td>
</tr>
<tr style="height: 23px;">
<td style="width: 105.891px; height: 23px;">Inter-frame Delay (IFD)</td>
<td style="width: 19.1094px; height: 23px;">3000ms&nbsp;</td>
</tr>
<tr style="height: 23px;">
<td style="width: 105.891px; height: 23px;">Minimum Frame Spacing (FSP<sub>MIN</sub>)</td>
<td style="width: 19.1094px; height: 23px;">750ms&nbsp;</td>
</tr>
</tbody>
</table>
<br>
Only the data from the detection algo is sent over CoAP (i.e state changes) together with some other stuff (ambient brightness, battery levels etc.).

## Communication
The IPR utilizes CoAP for low-power communication with a remote server. In this project, the server runs on the same hardware as the border router.
| Server (OTBR)         |                      | Client (IPR)       | Message                                        |
| ----------------------|:-------------------:|:-------------------:|:----------------------------------------------:|
|                       | <------------------ |        DNS-SD       | Register SLAAC IPv6 with SRP server            |
|         GET           | ------------------> |                     | Informs client of the server IP and uri        |
|                       | <------------------ |        ACK          | Ack (must be received or will not proceed)     |
|           .           |         .           |          .          |                       .                        |
|           .           |         .           |          .          |                       .                        |
|                       | <------------------ |        PUT          | If change of detection state AND periodically  |
|         ACK           | ------------------> |                     | Ack                                            |

It is noteworthy that CoAP has many security issues like MQTT, but they were not of particular concern in this application since CoAP communication never leaves mesh-local.<br><br>
![Communication](https://github.com/edward62740/ot-IPR/blob/master/Documentation/comm.png "Communication")
<br>
There is also IPv6 address discovery implemented over DNS-SD (RFC6763), to allow the CoAP server to discover nodes that are connected to other routers.

## Performance and Future Improvements
Currently, the sensor has an average power consumption of approx. 140-160uA @ 1.8v, which can be reduced at the cost of performance (shown below)<br>
![Power Consumption](https://github.com/edward62740/ot-IPR/blob/master/Documentation/pwr.png "Power Consumption")<br>
This gives the sensor a battery life of >1 year on 2 x LR03 cells.

Future improvements are to replace the sensor with the pin-compatible [A121](https://developer.acconeer.com/download/a121-datasheet-pdf/), which is an improved version of the radar sensor with significantly lower idle vtx/rx currents, and should offset the avg. current by -60uA. Another obvious improvement is to disable the sensor at night.
<br>
The CoAP server allows for data collected to be visualized as shown below: <br>
![Visualization](https://github.com/edward62740/ot-IPR/blob/master/Documentation/grafana.png "Visualization")<br>
