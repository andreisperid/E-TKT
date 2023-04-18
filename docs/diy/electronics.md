---
layout: default
title: ⚡ electronics
nav_order: 3
description: "E-TKT"
parent: 🛠️ do it yourself!
---

# ⚡ **electronics**

----

💬 *you can find the KiCad v6 source files, PCB 3D export, PDF schematics and gerbers in the [electronic](https://github.com/andreisperid/E-TKT/tree/main/_electronic/) folder.*

----

![_DSC0364](https://user-images.githubusercontent.com/15098003/196300472-0a6d9673-1229-49ee-814d-58a3f39551d4.jpg)

----

## Board
<img width="50%" src="https://user-images.githubusercontent.com/15098003/196300905-4247ead6-7743-4f2a-bca7-bb8238c75d28.png"><img width="50%" src="https://user-images.githubusercontent.com/15098003/196300906-cbcebdab-12e4-495c-bd3a-9b33c1dceccf.png">

----

## Schematics
![image](https://user-images.githubusercontent.com/15098003/193071371-ac9c6aa6-0b87-4183-b6b2-9ccb1d030f1d.png)  
<p align="right">
  <a href="https://github.com/andreisperid/E-TKT/blob/main/_electronic/export/etkt_schematic.pdf" target="_blank">
    <i>🔎 see larger</i>
  </a>
</p>  

----

## Specs
- ***Size:*** 99.6 x 76.2mm(3.92 x 3.00 inches)
- ***Layer count:*** 4
- ***Material:*** FR-4
- ***FR4-TG:*** TG 150-160
- ***Thickness:*** 1.6mm
- ***Min. Track/Spacing:*** 6/6mil
- ***Min. Hole Size:*** 0.3mm↑
- ***Solder Mask:*** Black
- ***Silkscreen:*** White
- ***Surface Finish:*** HASL
- ***Thickness:*** 1.6mm

----

## Subsystems

### Power
  - ***12V*** needs to be provided by an external power supply with at least 3A (real) to deal with servo peaks current while pressing the label. It supplies both the stepper drivers directly.
  - ***6V*** out of the bucket converter and is provided for both the servo and hall sensor. The peak voltage tested on oscilloscope was +1V surge when the servo stalls. *
  - ***3.3V*** is provided by the AZ1117 linear regulator, which receives power directly from the 6V rail.

***Important: Logic level MOSFETs (BSS138)*** were implemented because the ESP32 uses 3.3v logic, we need this conversion for parts that are running on 6V (servo and hall sensor).

<h6><i>* Using a bucket converter for the 6V rail is preferred due to the high step from 12V, but a cheaper alternative using a L7806 (1.5A) is oferred at the PCB back side. For that, you should not mount the bucket converter components (C_POW_IN_1, C_POW_IN_2, C_POW_IN_3, U_POW1, C_POW_BST_1, L1, C_POW_6V_FF1, R_POW_FBT1, R_POW_FFB1, C_POW_OUT_1, C_POW_OUT_2, C_POW_OUT_3, C_POW_OUT_4, C_POW_OUT_5). But IMPORTANT, this exact setup wasn't tested on this exact PCB (only in the preliminar prototype), so do it at your own risk.</i></h6>


### Processing & wireless communication
- Using an ESP32-WROOM-32D-N4, with an integrated antenna.

### Serial USB communication
- An FT231XS-R is used for flashing the firmware through an USB-C 2.0 port.

### Label press
- The MG996-R servo has a maximum constant voltage of 6V. Constant higher tension tends to damage it.

### Character selection (daisy wheel)
- A NEMA stepper driven by a A4988 breakout and a KY 003 hall sensor for homing the daisy wheel.

### Wifi reset
- When this tactile button is pressed while booting the saved wifi credentials are cleared.

### Feeder
- 12v reduced stepper motor (28BYJ-48) driven by a ULN2003 darlington array.

### Graphic user interface
- Running on 3.3v, an I²C OLED display and two DIP LEDs mounted into the J_top piece.

### Buzzer
- Optional magnetic buzzer that plays notes for audible feedback.

----

## Stackup
<p align="center">  
<img width="25%" src="https://user-images.githubusercontent.com/15098003/193071414-60c6b8d1-a408-4636-970f-516105952620.png"><img width="25%" src="https://user-images.githubusercontent.com/15098003/193071446-c8ad1838-c0e3-43a6-938a-168fd1d36d60.png"><img width="25%" src="https://user-images.githubusercontent.com/15098003/193071449-e0617bdb-d3d9-4312-b82b-2f735cb272e8.png"><img width="25%" src="https://user-images.githubusercontent.com/15098003/193071444-545b5f12-a920-4734-ae05-dccdc0a62cc8.png">
</p>

- **Front (F.Cu)**: most components + mixed signals & power + ground plane;
- **Inner 1 (In1.Cu)**: ground plane;
- **Inner 2 (In2.Cu)**: 6V plane;
- **Back (B.Cu)**: few components + headers + mixed signals and power + ground plane.


