---
layout: default
title: ⚡ electronics
nav_order: 3
description: "E-TKT"
parent: 🛠️ do it yourself!
---

# ⚡ **electronics**

💬 *you can find the KiCad v6 source files, PDF schematics and gerbers in the [electronic](https://github.com/andreisperid/E-TKT/tree/main/_electronic/) folder.*

----

## Board
<img width="50%" src="https://user-images.githubusercontent.com/15098003/193073510-e8ca0f93-9d0d-4541-8dd6-37c3eacae061.png"><img width="50%" src="https://user-images.githubusercontent.com/15098003/193073505-1d91c3db-753d-4110-8e7a-523b866b203b.png">

----

## Stackup
<p align="center">  
<img width="25%" src="https://user-images.githubusercontent.com/15098003/193071414-60c6b8d1-a408-4636-970f-516105952620.png"><img width="25%" src="https://user-images.githubusercontent.com/15098003/193071446-c8ad1838-c0e3-43a6-938a-168fd1d36d60.png"><img width="25%" src="https://user-images.githubusercontent.com/15098003/193071449-e0617bdb-d3d9-4312-b82b-2f735cb272e8.png"><img width="25%" src="https://user-images.githubusercontent.com/15098003/193071444-545b5f12-a920-4734-ae05-dccdc0a62cc8.png">
</p>

- **Front (F.Cu)**: most components + mixed signals & power + ground plane;
- **Inner 1 (In1.Cu)**: ground plane;
- **Inner 2 (In2.Cu)**: 6V plane;
- **Back (B.Cu)**: few components + headers + mixed signals and power + ground plane.

----

## Schematics
![image](https://user-images.githubusercontent.com/15098003/193071371-ac9c6aa6-0b87-4183-b6b2-9ccb1d030f1d.png)  
<p align="right">
  <a href="https://github.com/andreisperid/E-TKT/tree/main/_electronic/" target="_blank">
    <i>🔎 see larger</i>
  </a>
</p>
  
  

----
## Subsystems


### Power
  - ***12V*** needs to be provided by an external power supply with at least 3A (real) to deal with servo peaks current while pressing the label. It supplies both the stepper drivers directly.
  - ***6V*** out of the bucket converter and is provided for both the servo and hall sensor. The peak voltage tested on oscilloscope was +1V surge when the servo stalls. *
  - ***3.3V*** is provided by the AZ1117 linear regulator, which receives power directly from the 6V rail.

<h6><i>* Using a bucket converter for the 6V rail is preferred due to the high step from 12V, but a cheaper alternative using a L7806 (1.5A) is oferred at the PCB back side. For that, you should not mount the bucket converter components (C_POW_IN_1, C_POW_IN_2, C_POW_IN_3, U_POW1, C_POW_BST_1, L1, C_POW_6V_FF1, R_POW_FBT1, R_POW_FFB1, C_POW_OUT_1, C_POW_OUT_2, C_POW_OUT_3, C_POW_OUT_4, C_POW_OUT_5). But IMPORTANT, this exact setup wasn't tested on this exact PCB (only in the preliminar prototype), so do it at your own risk.</i></h6>


### Processing & wireless communication
- Using an ESP32-WROOM-32D-N4, with an integrated antenna.

### Serial communication
- An FT231XS-R is used for flashing the firmware through an USB-C 2.0 port.

### Logic Level MOSFETs (BSS138)
- As the ESP32 uses 3.3v logic, we need this conversion for parts that are running on 6V (servo and hall sensor).

### Press
- The MG996-R servo has a maximum constant voltage of 6V. Constant higher tension tends to damage it.

### Daisy wheel
- A NEMA stepper driven by a A4988 breakout and a KY 003 hall sensor for homing.

### Wifi reset
- When this tactile button is pressed while booting, the saved wifi credentials are cleared.

### Feeder
- 12v reduced stepper motor (28BYJ-48) driven by a ULN2003 darlington array.

### User interface
- Running on 3.3v, an I²C OLED display and two DIP LEDs mounted into the J_top piece.

### Buzzer
- Optional magnetic buzzer that plays notes for audible feedback.
