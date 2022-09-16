---
layout: default
title: electronics
nav_order: 3
description: "E-TKT"
parent: 🛠️ do it yourself!
---

# ⚡ **electronics**

### Schematics

![Schematic_e-tkt_2022-05-30](https://user-images.githubusercontent.com/15098003/171064999-262a4c68-01ae-4122-8584-5d784ebf6408.png)

### Power
  - *7-12v* to be provided by an external power supply with at least 35w to deal with servo peaks current while pressing the label. It supplies the stepper drivers directly.
  - *6v* out of the L7806 step down and is provided for both the servo and hall sensor.
  - *3.3v* is provided by the ESP32 WROOM board (as in its logical ports).

### Logic Level
- As the ESP32 uses 3.3v logic, we need this conversion for parts that are running on higher voltage (servo and hall sensor).

### Press
- The servo uses 6v, higher voltages tend to damage it.

### Carousel
- NEMA stepper, driver and a hall sensor to match the position origin.

### Wifi reset
- A tact button that when pressed while booting, clears the saved credentials.

### Feeder
- Reduced stepper motor along with its standard driver.

### User interface
- Running on 3.3v an I²C OLED display and two LEDs (no need for resistors).
