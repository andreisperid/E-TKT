<h1 align="center">📦 E-TKT: anachronic label maker 📦</h1>

<p align="center">Andrei Speridião 2022 - http://andrei.cc</p>

# What is it?

> ### *étiquette f (plural étiquettes)*
> ["ticket, memorandum, attach, stick, pierce, sting, to be sharp, goad, puncture, attach, nail"](https://en.wiktionary.org/wiki/%C3%A9tiquette#French)
> 1. tag, label 
> 2. prescribed behavior


E-TKT is a DIY label maker that mixes both old fashioned and contemporary technology to create something as simple as... Labels!

## [...see it working!](https://youtu.be/5hv-2kIJUVc)

# Table of contents

- [Why?](#-why)
- [How does it work?](#%EF%B8%8F-how-does-it-work)
- [Features](#-features)
- [To do](#%EF%B8%8F-to-do)
- [List of components](#-list-of-components)
- [3D printing](#-3d-printing)
- [Libraries](#-libraries)



# 🤔 Why?

### *TLDR: curiosity, technical challenge and of course organizing stuff.*

The initial spark for this project came from an ordinary handheld [labeling device](https://www.aliexpress.com/item/3256801648218535.html) that I bought to organize my workshop. I was ***VERY UPSET*** when I noticed that it was basically rubbish, getting broken and unusable after just a few operations.

> IMG motex

I knew I could easily buy a common electronic thermal label printer, but something about the vintage embossed finish fascinated me. Furthermore, even though the pressing mechanism was poorly made, the characters' carousel was pretty sturdy and sharp. So I thought: can I build over its principles to create a functional device? If so, then why not make it physical-digital?

Throughout the process I have constantly questioned what made me develop this ambiguous device, with no clear answer.

Finally, when creating the project video I've realized that "anachronism" is what I’ve been attracted to. According to [Wikipedia](https://en.wikipedia.org/wiki/Anachronism):
> “a chronological inconsistency in some arrangements, especially a juxtaposition of people, events, objects, language terms and customs from different time periods”.

### The point is: even though the process is digitalized, the resulting label is totally old school and there is no easy way of telling if it wasn't made in the traditional way.

...why bother mixing an archaic printing method with current automation and connectivity features? Because why not? It was a technological blind spot to be explored!



# ⚙️ How does it work?

An ESP32 commands the label production and also serves an on demand web application to any device connected in a local network. Neither Internet is needed, nor installing any app.

The whole process of connecting the E-TKT machine to a local network and then launching the app is aided by a small OLED screen that provides instructions and a dynamically generated QR code with the URL, according to the IP attributed by the WLAN.

The web app provides text validation, special characters, a preview of the exact size of the tape, an option to select the desired lateral margins and also real-time feedback during the printing (also present on the device screen). There are also commands for attaching a new reel, manually feeding and cutting the tape.

The label production itself uses the same mechanical principles as the original machine did, but is now automated. A stepper motor feeds the tape while another selects the appropriate character on the carousel according to a home position acquired by hall sensor. Then a servo motor imprints each character by pressing the carousel to the tape. That happens successively until the end of the desired content, when there is a special character position to cut the label. A light blinks to ask for the label to be picked.


# 💡 Features
### 🌟 *Highlights*
- No need for internet, app installation, data cables or drivers;
- Use from any device that is connected to a local network and has a web browser: desktop, tablet, smartphone;
- Compatible with cheap 9mm generic DYMO-compatible tape;

### 📱 Web app
> IMG GIF: overview, margins, validation

- Instant preview: what you see is what you’ll get;
- Real time check for character validity;
- Label length estimation;
- Margin modes: tight, small (1 space each side) or full (max input length);
- Buttons for special characters: ♡  ☆  ♪  $ @
- Reeling function: for when a new tape reel must be installed;
- Manual commands: feed & cut;

### 🔌 Device
> IMG overview
> IMG OLED display

- Minimum label size to allow for picking it up;
- OLED screen + LED feedback:
- Instructions for configuring the wifi;
- QR code/URL for easily accessing the web app;
- Real time progress;


# 🛠️ To do
- [ ] Bugs
    - [ ] (device) The machine seldom mistakes the correct letter (rare), seems related to processing hiccups.
    - [ ] (device) Restarts occur occasionally, and it seems related to processing peaks that trigger the task watchdog.
    - [ ] (app, device) Sometimes the status feedback (reel, cut, feed, print) does not update correctly and gets stuck.
    - [ ] (app, device) Frequently the font serving from the ESP32 takes more time than usual and the web app starts without the correct aspect, eventually getting stuck in that state.
- [ ]  Manufactured PCB.

***<details><summary>🕓 See the previous development phases 🕕</summary>***
<p>
    
### I - Experiment (March 2021)
- [x] Centered carousel with oversized NEMA stepper
- [x] Carousel homing with infrared sensor + one missing “teeth” led erratic results
- [x] Testing with servo, but it was too weak to press the tape
- [x] Using Arduino mega, communication via serial

### II - Communication
- [x] Experiments using ESP8266 with self served app + receiving commands, still isolated from the main functionality
- [x] Wifi manager
- [x] First sketches for the web app user interface

###  III - Printing Proof
- [x] Migration for the ESP32
- [x] Dual core tasks: one serves the network/app and the other controls all physical operations
- [x] Stronger servo resulted on successful tag printing
- [x] Hall sensor for homing with precision
- [x] Carousel direction on clockwise only to avoid tape screwing

###  IV - Consistency and Usability
- [x] More compact device, with smaller stepper motor for the carousel
- [x] OLED display to help on configuration and feedback
- [x] Improvements on web app usability and visuals

### V - Optimization, Extras and Documentation (May 2022)
- [x] Implemented special symbols
- [x] Few printing optimizations
- [x] Documentation with texts, schemes, photos and video
    
</p>
</details>




# 🧩 List of components

<!--The estimated cost is around $70 (USD) without shipping.-->
 
| TYPE | PART - DESCRIPTION | QTY |
| :--- |  :--- | :---: |
| structure | 3D print filament - PETG (~200g used) | 0.2 |
| structure | screw - M3x40mm | 1 |
| structure | screw - M3x20mm | 17 |
| structure | screw - M3x18mm | 3 |
| structure | screw - M3x16mm | 2 |
| structure | screw - M3x8mm | 6 |
| structure | screw - M3x6mm | 10 |
| structure | screw - M4x12mm | 1 |
| structure | hex nut - M3 | 10 |
| structure | washer - M3 | 9 |
| structure | silicone pad feet - 8x2mm | 4 |
| mechanic | spring - 7mm ⌀ x 6mm length | 1 |
| mechanic | pressing carousel - MOTEX / CIDY | 1 |
| mechanic | bearing - 608ZZ | 1 |
| mechanic | bearing - 623ZZ | 2 |
| mechanic | extruder gear - MK8 40 teeth / 5mm axis | 1 |
| electronic | dual side prototyping PCB - 0.1" pitch - 50x70x1.6mm | 1 |
| electronic | hall sensor - KY-003 | 1 |
| electronic | stepper motor - NEMA 17HS4023 | 1 |
| electronic | stepper driver - A4988 | 1 |
| electronic | stepper motor (28BYJ-48 5V) & driver (ULN2003) | 1 |
| electronic | servo - Towerpro MG-996R | 1 |
| electronic | neodymium magnet - 2mm ⌀ x 3mm length | 1 |
| electronic | ESP32 WROOM nodeMCU | 1 |
| electronic | 4-channel I2C-safe Bi-directional Logic Level Converter - BSS138 | 1 |
| electronic | OLED screen - 128x32px 0.91" | 1 |
| electronic | LED - white PTH 2x5x7mm square | 2 |
| electronic | step down - 6V - LM7806 | 1 |
| electronic | power supply - 7V 5A | 1 |
| electronic | DC-005 Power Jack Socket | 1 |
| electronic | tact switch - 6x6x4.5 | 1 |
| electronic | wiring - solid (prototype) | 1 |
| harness | USB type-A to micro-B data cable | 1 |
| harness | wiring (harnesses) - flexible, flat | 1 |
| harness | header jumper connector | 1 |
| harness | male header - 0.1" pitch | 2 |
| harness | female header - 0.1" pitch | 2 |
| harness | female connector - 8 pin 0.1" pitch dupont | 1 |
| harness | female connector - 3 pin 0.1" pitch dupont | 4 |
| harness | female connector - 4 pin 0.1" pitch dupont | 2 |
  
*For more info, please access the [B.O.M. spreadsheet](https://docs.google.com/spreadsheets/d/1Xqc29A99QCQw1OPSw2hl-alj2u_e9j2tNaFo3Z4aVT0/edit?usp=sharing).*



# ⚡ Electronics

> IMG schematics

### Power
  - *7-12v* to be provided by an external power supply with at least 35w to deal with servo peaks current while pressing the label. It supplies the stepper drivers directly.
  - *6v* out of the L7806 step down and is provided for both the servo and hall sensor.
  - *3.3v* is provided by the ESP32 WROOM board (as in its logical ports).

### Logic Level
- as the ESP32 uses 3.3v logic, we need this conversion for parts that are running on higher voltage (servo and hall sensor).

### Press
- the servo uses 6v, higher voltages tend to damage it.

### Carousel
- NEMA stepper, driver and a hall sensor to match the position origin.

### Wifi reset
- a tact button that when pressed while booting, clears the saved credentials.

### Feeder
- reduced stepper motor along with its standard driver.

### User interface
- running on 3.3v an I²C OLED display and two LEDs (no need for resistors).



# 🧵 3D printing
> IMG exploded

16 parts in total, using approx 200g of PETG filament.

### List of parts
- A_bottom
- B_wall
- C_wall_track
- D_pillar_1
- E_pillar_2
- F_pillar_3
- G_pillar_4
- H_pillar_5
- I_top
- J_top_screenholder
- K_top_tapefeed
- L_caroulsel_cube
- M_carousel_hallholder
- N_carousel_coupling_1
- O_carousel_coupling_2
- P_press

### Settings
- layer height: 0.25mm
- infill: 20%
- wall line count: 3 + 1 (Cura usually adds the later together with the infill)
- top/bottom layers: 4

*PS: some parts might need support.*



# 📚 Libraries

Framework: arduino

Environment: nodemcu-32s @3.3.2

- [waspinator/AccelStepper@^1.61](https://github.com/waspinator/AccelStepper )

- [madhephaestus/ESP32Servo@^0.9.0](https://github.com/madhephaestus/ESP32Servo )

- [ottowinter/ESPAsyncWebServer-esphome@^1.2.7](https://github.com/me-no-dev/ESPAsyncWebServer)

- [alanswx/ESPAsyncWiFiManager@^0.24](https://github.com/alanswx/ESPAsyncWiFiManager)

- [olikraus/U8g2@^2.28.8](https://github.com/olikraus/U8g2)

- [ricmoo/QRCode@^0.0.1](https://github.com/ricmoo/QRCode) 


# License

MIT @ [Andrei Speridião](https://github.com/andreisperid/)

If you ever build one, I would love to know ;)

## ...and you are more than welcome to visit [andrei.cc](https://andrei.cc) !
