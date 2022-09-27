---
layout: default
title: ☑️ bill of materials
nav_order: 4
description: "E-TKT"
parent: 🛠️ do it yourself!
---

# ☑️ **bill of materials**

The estimated cost is around $?? (USD) without shipping and taxes, as of October 2022.
 
## A. structure & mechanic
 
 
| ID | TYPE | SPEC | REF | QTY |
| :---: | :---: | :--- | :--- | :---: |
|  1 | 3D print | filament — PETG | [link](http://prusa3d.com/product/prusament-petg-jet-black-1kg/) | ~200g |
|  2 | screw | cyl. hex steel — M3x40mm | [link](http://aliexpress.com/item/2261799963738734.html) |  |
|  3 | screw | cyl. hex steel — M3x20mm | [link](http://aliexpress.com/item/2261799963738734.html) |  |
|  4 | screw | cyl. hex steel — M3x18mm | [link](http://aliexpress.com/item/2261799963738734.html) |  | 
|  5 | screw | cyl. hex steel — M3x16mm | [link](http://aliexpress.com/item/2261799963738734.html) |  |
|  6 | screw | cyl. hex steel — M3x8mm | [link](http://aliexpress.com/item/2261799963738734.html) |  | 
|  7 | screw | cyl. hex steel — M3x6mm | [link](http://aliexpress.com/item/2261799963738734.html) |  | 
|  8 | screw | cyl. hex steel — M4x12mm | [link](http://aliexpress.com/item/2261799963738734.html) | 1 | 
|  9 | nut | hex steel — M3 | [link](http://aliexpress.com/item/1005001966426139.html) |  | 
| 10 | washer | steel — M3 | [link](http://aliexpress.com/item/3256801295230574.html) |  | 
| 10 | washer | steel — M4 | [link](http://aliexpress.com/item/3256801295230574.html) | 1 | 
| 11 | silicone pad feet | 8x2mm | [link](http://aliexpress.com/item/2251832637568894.html) | 4 | 
| 12 | daisy wheel | MOTEX / CIDY |  [link](https://www.aliexpress.com/item/1005004489017299.html) | 1 | 
| 13 | bearing | 608ZZ | [link](http://aliexpress.com/item/1005001813219171.html) | 1 | 
| 14 | bearing | 623ZZ | [link](http://aliexpress.com/item/1005001813219171.html) | 2 | 
| 15 | extruder gear | MK8 40 teeth / 5mm axis | [link](http://aliexpress.com/item/2255800252771556.html) | 1 |

 
## B. sensors & actuators

| ID | SUBSYSTEM | PART - DESCRIPTION | QTY | REF | 
| :---: | :---: |  :--- | :---: | :---: 
| 1 | daisy wheel | hall sensor - KY-003 | 1 | [link](http://aliexpress.com/item/2251832475321023.html) |
| 2 | daisy wheel | stepper motor - NEMA 17HS4023 | 1 | [link](http://aliexpress.com/item/2251832620474591.html) |
| 3 | daisy wheel | stepper driver - A4988 | 1 | [link](http://aliexpress.com/item/3256801435362018.html) |
| 4 | extruder | stepper motor (28BYJ-48 5V) & driver (ULN2003) | 1 | [link](http://aliexpress.com/item/1005003353402464.html) |
| 5 | press | servo - Towerpro MG-996R | 1 | [link](http://aliexpress.com/item/2251832857187114.html) |
| 6 | daisy wheel | neodymium magnet - 2mm ⌀ x 3mm length | 1 | [link](http://aliexpress.com/item/3256803632497346.html) |

## C. PCB & components

| ID | TYPE | SPEC — PACKAGE | REF | QTY |
| :---: | :---: | :--- | :--- | :---: |
|  1 | PCB | 4 layer, FR4, black + front stencil — 99.57 mm x 76.2 mm | | 1 |
|  2 | MCU | ESP32 — WROOM-32D (SMD) | [ESP32-WROOM-32D-N4](https://www.findchips.com/search/ESP32-WROOM-32D-N4) | 1 |
|  3 | SERIAL | USB Interface, Full Serial UART — SSOP-20 3.9x8.7mm (SMD) | [FT231XS-R](https://www.findchips.com/search/FT231XS-R) | 1 |
|  4 | DRIVER | Darlington 50v 500mA — SOIC-16 3.9x9.9mm (SMD) | [ULN2003ADRE4](https://www.findchips.com/search/ULN2003ADRE4) | 1 |
|  5 | REGULATOR | Switching, 4.5-28V 3A Output — SOT-23-6 (SMD) | [TPS54302](https://www.findchips.com/search/TPS54302) | 1 |
|  6 | REGULATOR | LDO 3.3V 800mA — SOT-223 (SMD) | [AZ1117CH-3.3TRG1](https://www.findchips.com/search/AZ1117CH-3.3TRG1) | 1 |
|  7 | TVS DIODE | USB6B1 — SOIC-8 3.9x4.9mm (SMD) | [USB6B1RL](https://www.findchips.com/search/USB6B1RL) | 1 |
|  8 | INDUCTOR | 10uH 20% 7A — SRP6060 (SMD) | [SRP6060FA-100M](https://www.findchips.com/search/SRP6060FA-100M) | 1 |
|  9 | BUZZER | Electromagnetic 3V — CSS-0575A (SMD) | [CSS-0575A-SMT](https://www.findchips.com/search/CSS-0575A-SMT) | 1 |
| 10 | MOSFET | N-CH 0.22A, 50V, 1.5V — SOT-23 (SMD) | [BSS138LT3G](https://www.findchips.com/search/BSS138LT3G) | 2 |
| 11 | BJT | 625mW, 25V, 1.5A — SOT-23 (SMD) | [MMSS8050-L-TP](https://www.findchips.com/search/MMSS8050-L-TP) | 2 |
| 12 | BUTTON | OFF-(ON) — 6X6X3.1mm (SMD) | [B3FS-1000](https://www.findchips.com/search/B3FS-1000) | 3 |
| 13 | FERRITE | 40 Ohms 5A — 0805 (SMD) | [MH2029-400Y](https://www.findchips.com/search/MH2029-400Y) | 1 |
| 14 | FUSE | 6V 750mA — 0805 (SMD) | [0ZCK0075FF2E](https://www.findchips.com/search/0ZCK0075FF2E) | 1 |
| 15 | LED | LED white — 0603 (SMD) | [SMLD12WBN1W1](https://www.findchips.com/search/SMLD12WBN1W1) | 12 |
| 16 | CONNECTOR | USB-C 2.0 — GCT USB4105-GF-A | [GCT USB4105-GF-A](https://www.findchips.com/search/USB4105-GF-A) | 1 |
| 17 | CONNECTOR | Barrel Jack 2mm/6.3mm, 5A 20V — DCJ200 | [DCJ200-10-A-K1-K](https://www.findchips.com/search/DCJ200-10-A-K1-K) | 1 |
| 18 | CONNECTOR | Conn_01x02_Male — Header 1x02 P2.54mm Vertical | [M20-9990245](https://www.findchips.com/search/M20-9990245) | 2 |
| 19 | CONNECTOR | Conn_01x03_Male — Header 1x03 P2.54mm Vertical | [M20-9990346](https://www.findchips.com/search/M20-9990346) | 1 |/
| 20 | CONNECTOR | Conn_01x04_Male — Header 1x04 P2.54mm Vertical | [M20-9770446](https://www.findchips.com/search/M20-9770446) | 1 |
| 21 | CONNECTOR | Conn_01x05_Male — Header 1x05 P2.54mm Vertical | [M20-9990546](https://www.findchips.com/search/M20-9990546) | 1 |
| 22 | CONNECTOR | Conn_01x04_Male — Header 1x04 P2.54mm Horizontal | [M20-9750446](https://www.findchips.com/search/M20-9750446) | 1 |
| 23 | CONNECTOR | Conn_01x03_Female — Socket 1x03 P2.54mm Vertical | [M20-7820346](https://www.findchips.com/search/M20-7820346) | 1 |
| 24 | CONNECTOR | Conn_02x08_Female — Socket 2x08 P2.54mm Vertical (A4988) | [M20-7820842](https://www.findchips.com/search/M20-7820842) | 2 |
| 25 | CAPACITOR | Electrolytical 100uF 25V — 6.3x7.7 (SMD) | [EMVE250ADA101MF80G](https://www.findchips.com/search/EMVE250ADA101MF80G) | 1 |
| 26 | CAPACITOR | Ceramic 22uF 25V — 1210 (SMD) | [C3225X7R1E226M250AB](https://www.findchips.com/search/C3225X7R1E226M250AB) | 8 |
| 27 | CAPACITOR | Ceramic 10uF 12V — 0603 (SMD) | [GRM188Z71A106KA73D](https://www.findchips.com/search/GRM188Z71A106KA73D) | 6 |
| 28 | CAPACITOR | Ceramic 0.1uF 50V — 0603 (SMD) | [C1608X7R1H104K080AA](https://www.findchips.com/search/C1608X7R1H104K080AA) | 6 |
| 29 | CAPACITOR | Ceramic 47pF 5V — 0603 (SMD) | [CGA3E2C0G1H470J080AA](https://www.findchips.com/search/CGA3E2C0G1H470J080AA) | 3 |
| 30 | RESISTOR | Thick Film 100K — 0603 (SMD) | [ERJ-3EKF1003V](https://www.findchips.com/search/ERJ-3EKF1003V) | 1 |
| 31 | RESISTOR | Thick Film 11K — 0603 (SMD) | [ERJ-3EKF1102V](https://www.findchips.com/search/ERJ-3EKF1102V) | 1 |
| 32 | RESISTOR | Thick Film 10K — 0603 (SMD) | [ERJ-3EKF1002V](https://www.findchips.com/search/ERJ-3EKF1002V) | 9 |
| 33 | RESISTOR | Thick Film 5K1 — 0603 (SMD) | [ERJ-3EKF5101V](https://www.findchips.com/search/ERJ-3EKF5101V) | 7 |
| 34 | RESISTOR | Thick Film 2K — 0603 (SMD) | [ERJ-3EKF2001V](https://www.findchips.com/search/ERJ-3EKF2001V) | 1 |
| 35 | RESISTOR | Thick Film 100R — 0603 (SMD) | [ERJ-3EKF1000V](https://www.findchips.com/search/ERJ-3EKF1000V) | 6 |
| 36 | RESISTOR | Thick Film 27R — 0603 (SMD) | [ERJ-3EKF27R0V](https://www.findchips.com/search/ERJ-3EKF27R0V) | 4 |
		

## D. harnesses

| ID | SUBSYSTEM | PART - DESCRIPTION | QTY | REF | 
| :---: | :---: |  :--- | :---: | :---: |
| 1 | - | USB type-A to micro-B data cable | 1 | [link](http://aliexpress.com/item/2255800229926282.html) |
| 2 | - | wiring (harnesses) - flexible, flat | 1 | [link](http://aliexpress.com/item/2251832639497810.html) |
| 3 | - | header jumper connector | 1 | [link](http://aliexpress.com/item/2251801839907761.html) |
| 4 | - | male header - 0.1" pitch | 2 | [link](http://aliexpress.com/item/2251832538163556.html) |
| 5 | - | female header - 0.1" pitch | 2 | [link](http://aliexpress.com/item/2251832538163556.html) |
| 6 | - | female connector - 8 pin 0.1" pitch dupont | 1 | [link](http://aliexpress.com/item/3256802073547679.html) |
| 7 | - | female connector - 3 pin 0.1" pitch dupont | 4 | [link](http://aliexpress.com/item/3256802073547679.html) |
| 8 | - | female connector - 4 pin 0.1" pitch dupont | 2 | [link](http://aliexpress.com/item/3256802073547679.html) |

## E. other

| ID | SUBSYSTEM | PART - DESCRIPTION | QTY | REF | 
| :---: | :---: |  :--- | :---: | :---: |
| 1 | power | 12v 3A (real) | 1 | |
| 2 | consumables | label tape - 9mm DYMO compatible | 🦝 | [link](http://aliexpress.com/item/1005001525284316.html) |
