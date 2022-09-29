---
layout: default
title: ⚡ electronics
nav_order: 3
description: "E-TKT"
parent: 🛠️ do it yourself!
---

# ⚡ **electronics**

You can find the KiCad v6, schematics and gerbers in the [electronic](https://github.com/andreisperid/E-TKT/tree/main/_electronic/) folder.

### Schematics
![image](https://user-images.githubusercontent.com/15098003/193057365-5c53cc35-9b48-42b0-890c-034a05460801.png)

### Power rails
  - ***12V*** needs to be provided by an external power supply with at least 3A (real) to deal with servo peaks current while pressing the label. It supplies both the stepper drivers directly.
  - ***6V*** out of the bucket converter and is provided for both the servo and hall sensor. The peak voltage tested on oscilloscope was +1V surge when the servo stalls. *
  - ***3.3V*** is provided by the AZ1117 linear regulator, which receives power directly from the 6V rail.

<h6><i>* Using a bucket converter for the 6V rail is preferred due to the high step from 12V, but a cheaper alternative using a L7806 (1.5A) is oferred at the PCB back side. For that, you should not mount the bucket converter components (C_POW_IN_1, C_POW_IN_2, C_POW_IN_3, U_POW1, C_POW_BST_1, L1, C_POW_6V_FF1, R_POW_FBT1, R_POW_FFB1, C_POW_OUT_1, C_POW_OUT_2, C_POW_OUT_3, C_POW_OUT_4, C_POW_OUT_5). But IMPORTANT, this exact setup wasn't tested on this exact PCB (only in the preliminar prototype), so do it at your own risk.</i></h6>


### ESP32-D WROOM
- Version ESP32-WROOM-32D-N4, with integrated antenna.

### Serial communication
- An FT231XS-R for flashing the firmware through an USB-C 2.0 port.

### Logic Level MOSFETs (BSS138)
- As the ESP32 uses 3.3v logic, we need this conversion for parts that are running on 6V (servo and hall sensor).

### Press
- The MG996-R servo has a maximum constant voltage of 6V. Constant higher tension tends to damage it.

### Daisy wheel
- A NEMA stepper driven by a A4988 breakout and a KY 003 hall sensor for homing.

### Wifi reset
- A tactile button that when pressed while booting, clears the saved wifi credentials.

### Feeder
- 12v reduced stepper motor (28BYJ-48) driven by a ULN2003 darlington array.

### User interface
- Running on 3.3v, an I²C OLED display and two DIP LEDs mounted into the J_top piece.

### Buzzer
- Optional magnetic buzzer that plays notes for audible feedback.
