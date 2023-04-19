---
layout: default
title: 📝 bill of materials
nav_order: 4
description: "E-TKT"
parent: 🛠️ do it yourself!
---

# 📝 **bill of materials**

----

![_DSC0022](https://user-images.githubusercontent.com/15098003/196436665-9944830a-e542-4ce0-8121-35b3681ad0ac.jpg)

----

## 💬 cost per unit

The estimated cost for the components (without attrition) is around **$100 (USD) without shipping and taxes**, as of October 2022.

This value takes into account all the breaking points for a single E-TKT device and includes the manufacturing of 5 PCBs (4 layers) plus a stencil for the front layer.

----

## A. structure & mechanic 

| TYPE | SPEC | REF | QTY |
| :--- | :--- | :---: | :---: |
| 3D print | filament — PETG | [link](http://prusa3d.com/product/prusament-petg-jet-black-1kg/) | ~220g |
| socket head screw | ISO 4762 - M3 x 40 - A2-70 | [link](http://aliexpress.com/item/2261799963738734.html) | 1 |
| socket head screw | ISO 4762 - M3 x 20 - A2-70 | [link](http://aliexpress.com/item/2261799963738734.html) | 18 |
| socket head screw | ISO 4762 - M3 x 16 - A2-70 | [link](http://aliexpress.com/item/2261799963738734.html) | 7 |
| socket head screw | ISO 4762 - M3 x 10 - A2-70 | [link](http://aliexpress.com/item/2261799963738734.html) | 4 |
| socket head screw | ISO 4762 - M3 x 8 - A2-70 | [link](http://aliexpress.com/item/2261799963738734.html) | 2 | 
| socket head screw | ISO 4762 - M3 x 6 - A2-70 | [link](http://aliexpress.com/item/2261799963738734.html) | 9 | 
| socket head screw | ISO 4762 - M4 x 10 - A2-70 | [link](http://aliexpress.com/item/2261799963738734.html) | 1 | 
| hex nut | DIN EN ISO 4032 - M3 - A2-70 | [link](http://aliexpress.com/item/1005001966426139.html) | 10 | 
| washer | DIN EN ISO 7090 - 3 - 200 HV - A2 | [link](https://aliexpress.com/item/1005003991819416.html) | 9 | 
| washer | DIN EN ISO 7090 - 6 - 200 HV - A2 | [link](https://aliexpress.com/item/1005003991819416.html) | 1 | 
| silicone pad feet | ⌀8x2mm | [link](http://aliexpress.com/item/2251832637568894.html) | 4 | 
| daisy wheel | MOTEX — uppercase, 43 chars. |  [link](https://www.aliexpress.com/item/1005004489017299.html) | 1 | 
| deep groove ball bearing | 608-2Z | [link](https://www.aliexpress.com/w/wholesale-608%2525252d2z-bearing.html) | 1 | 
| deep groove ball bearing | 623-2Z | [link](https://www.aliexpress.com/w/wholesale-623%2525252d2z-bearing.html) | 2 | 
| extruder gear | MK8 40 teeth / 5mm axis | [link](http://aliexpress.com/item/2255800252771556.html) | 1 |

----
 
## B. sensors & actuators

| TYPE | SPEC | REF | QTY |
| :--- | :--- | :---: | :---: |
| hall sensor (breakout) | Keyes KY 003 | [link](http://aliexpress.com/item/2251832475321023.html) | 1 | 
| stepper driver (breakout) | A4988 | [link](http://aliexpress.com/item/3256801435362018.html) | 1 |
| stepper motor | NEMA 17HS4023 + cable | [link](http://aliexpress.com/item/2251832620474591.html) | 1 |
| stepper motor | 28BYJ-48 12V | [link](https://www.aliexpress.com/item/1005003708064424.html) | 1 | 
| servo | Towerpro MG-996R | [link](http://aliexpress.com/w/wholesale-Towerpro-MG%2525252d996R.html) | 1 | 
| neodymium magnet | ⌀2mm x 3mm length | [link](https://aliexpress.com/item/1005003022293200.html) | 1 |
| LED | Clear white —	DIP 2x5x7mm | [link](https://www.aliexpress.com/item/1005003347290499.html) | 1 |
| OLED display | SSD1306, 128x64, monochrome white — 4 pin I²C | [link](https://www.aliexpress.com/item/1005003484018034.html) | 1 |

----

## C. PCB & components

| TYPE | SPEC — PACKAGE | REF | QTY |
| :--- | :--- | :---: | :---: |
| PCB | 4 layers, FR4, black solder mask — 99.57 mm x 76.2 mm | | 1 |
| stencil | top layer only (F.Cu) — 200mm x 150mm | | 1 |
| MCU | ESP32 — WROOM-32D (SMD) | [ESP32-WROOM-32D-N4](https://www.findchips.com/search/ESP32-WROOM-32D-N4) | 1 |
| serial | USB Interface, Full Serial UART — SSOP-20 3.9x8.7mm (SMD) | [FT231XS-R](https://www.findchips.com/search/FT231XS-R) | 1 |
| driver | Darlington 50v 500mA — SOIC-16 3.9x9.9mm (SMD) | [ULN2003ADRE4](https://www.findchips.com/search/ULN2003ADRE4) | 1 |
| regulator | Switching, 4.5-28V 3A Output — SOT-23-6 (SMD) | [TPS54302](https://www.findchips.com/search/TPS54302) | 1 |
| regulator | LDO 3.3V 800mA — SOT-223 (SMD) | [AZ1117CH-3.3TRG1](https://www.findchips.com/search/AZ1117CH-3.3TRG1) | 1 |
| TVS diode | USB6B1 — SOIC-8 3.9x4.9mm (SMD) | [USB6B1RL](https://www.findchips.com/search/USB6B1RL) | 1 |
| inductor | 10uH 20% 7A — SRP6060 (SMD) | [SRP6060FA-100M](https://www.findchips.com/search/SRP6060FA-100M) | 1 |
| buzzer | Electromagnetic 3V — CSS-0575A (SMD) | [CSS-0575A-SMT](https://www.findchips.com/search/CSS-0575A-SMT) | 1 |
| MOSFET | N-CH 0.22A, 50V, 1.5V — SOT-23 (SMD) | [BSS138LT3G](https://www.findchips.com/search/BSS138LT3G) | 2 |
| BJT | 625mW, 25V, 1.5A — SOT-23 (SMD) | [MMSS8050-L-TP](https://www.findchips.com/search/MMSS8050-L-TP) | 2 |
| button | OFF-(ON) — 6X6X3.1mm (SMD) | [B3FS-1000](https://www.findchips.com/search/B3FS-1000) | 3 |
| ferrite | 40 Ohms 5A — 0805 (SMD) | [MH2029-400Y](https://www.findchips.com/search/MH2029-400Y) | 1 |
| fuse | 6V 750mA — 0805 (SMD) | [0ZCK0075FF2E](https://www.findchips.com/search/0ZCK0075FF2E) | 1 |
| LED | LED white — 0603 (SMD) | [SMLD12WBN1W1](https://www.findchips.com/search/SMLD12WBN1W1) | 12 |
| capacitor | Electrolytical 100uF 25V — 6.3x7.7 (SMD) | [EMVE250ADA101MF80G](https://www.findchips.com/search/EMVE250ADA101MF80G) | 1 |
| capacitor | Ceramic 22uF 25V X7R — 1210 (SMD) | [C3225X7R1E226M250AB](https://www.findchips.com/search/C3225X7R1E226M250AB) | 8 |
| capacitor | Ceramic 10uF 12V X7R — 0603 (SMD) | [GRM188Z71A106KA73D](https://www.findchips.com/search/GRM188Z71A106KA73D) | 6 |
| capacitor | Ceramic 0.1uF 50V X7R — 0603 (SMD) | [C1608X7R1H104K080AA](https://www.findchips.com/search/C1608X7R1H104K080AA) | 6 |
| capacitor | Ceramic 47pF 5V X7R — 0603 (SMD) | [CGA3E2C0G1H470J080AA](https://www.findchips.com/search/CGA3E2C0G1H470J080AA) | 3 |
| resistor | Thick Film 100K — 0603 (SMD) | [ERJ-3EKF1003V](https://www.findchips.com/search/ERJ-3EKF1003V) | 1 |
| resistor | Thick Film 11K — 0603 (SMD) | [ERJ-3EKF1102V](https://www.findchips.com/search/ERJ-3EKF1102V) | 1 |
| resistor | Thick Film 10K — 0603 (SMD) | [ERJ-3EKF1002V](https://www.findchips.com/search/ERJ-3EKF1002V) | 9 |
| resistor | Thick Film 5K1 — 0603 (SMD) | [ERJ-3EKF5101V](https://www.findchips.com/search/ERJ-3EKF5101V) | 7 |
| resistor | Thick Film 2K — 0603 (SMD) | [ERJ-3EKF2001V](https://www.findchips.com/search/ERJ-3EKF2001V) | 1 |
| resistor | Thick Film 100R — 0603 (SMD) | [ERJ-3EKF1000V](https://www.findchips.com/search/ERJ-3EKF1000V) | 6 |
| resistor | Thick Film 27R — 0603 (SMD) | [ERJ-3EKF27R0V](https://www.findchips.com/search/ERJ-3EKF27R0V) | 4 |
| connector | USB-C 2.0 — GCT USB4105-GF-A | [GCT USB4105-GF-A](https://www.findchips.com/search/USB4105-GF-A) | 1 |
| connector | Barrel Jack 2mm/6.3mm, 5A 20V — DCJ200 | [DCJ200-10-A-K1-K](https://www.findchips.com/search/DCJ200-10-A-K1-K) | 1 |
| connector | Conn_01x02_Male — Header 1x02 P2.54mm Vertical | [M20-9990245](https://www.findchips.com/search/M20-9990245) | 2 |
| connector | Conn_01x03_Male — Header 1x03 P2.54mm Vertical | [M20-9990346](https://www.findchips.com/search/M20-9990346) | 2 |
| connector | Conn_01x04_Male — Header 1x04 P2.54mm Vertical | [M20-9770446](https://www.findchips.com/search/M20-9770446) | 1 |
| connector |Conn_01x05_Male — Header 1x05 P2.54mm Vertical | [M20-9990546](https://www.findchips.com/search/M20-9990546) | 1 |
| connector | Conn_01x04_Male — Header 1x04 P2.54mm Horizontal | [M20-9750446](https://www.findchips.com/search/M20-9750446) | 1 |
| connector | Conn_02x08_Female — Socket 2x08 P2.54mm Vertical (A4988) | [M20-7820842](https://www.findchips.com/search/M20-7820842) | 2 |


### \* *Alternative 6V power*

| TYPE | SPEC — PACKAGE | REF | QTY |
| :--- | :--- | :---: | :---: |
| capacitor | Ceramic 0.33uF 50V X7R — 0603 (SMD) | [CGA3E3X7R1H334M080AB](https://www.findchips.com/search/CGA3E3X7R1H334M080AB) | 1 |
| capacitor | Ceramic 0.1uF 50V X7R — 0603 (SMD) | [C1608X7R1H104K080AA](https://www.findchips.com/search/C1608X7R1H104K080AA) | 1 |
| regulator | Linear 6v 1.5A — TO-220 | [L7806CV](https://www.findchips.com/search/L7806CV) | 1 |

----

## D. harnesses

| TYPE | SPEC | REF | QTY |
| :--- | :--- | :---: | :---: |
| cable | USB A to C data cable (firmware flash only) | | 1 |
| wiring | flat, flexible | [link](http://aliexpress.com/item/2251832639497810.html) | 1 |
| connector | female - 1x02 P2.54mm dupont | [link](http://aliexpress.com/item/3256802073547679.html) | 2 |
| connector | female - 1x04 P2.54mm dupont | [link](http://aliexpress.com/item/3256802073547679.html) | 2 |

----

## E. other

| TYPE | SPEC | REF | QTY |
| :--- | :--- | :---: | :---: |
| power | 12V 3A (real) 5,5mm x 2,1mm barrel jack, center positive | [link](https://aliexpress.com/item/1005002620565526.html) | 1 |
| label tape | 9mm DYMO-compatible | [link](https://www.aliexpress.com/item/1005002288190756.html) | 🦝 |
