---
layout: default
title: ⚙️ how does it work?
nav_order: 4
description: "E-TKT"
---

# ⚙️ **how does it work?**

----

<img src="https://user-images.githubusercontent.com/15098003/196457261-b6343e3f-528d-4cbe-a548-1b4f4463e4a1.png">

An *ESP32* commands the label production and also serves an on demand web application (local) to any device connected in a local network.

***Neither internet is needed, nor installing any app.***

### Configuration

The whole process of connecting the E-TKT machine to a local network and then launching the local app is aided by a small ***OLED display*** that provides instructions and a dynamically generated *QR code* with the *URL*, according to the IP attributed by the WLAN.

### Commands

The ***web app*** provides instant text validation, buttons for special characters, a preview of the physical size of the tape, an option to select the desired lateral margins and also real time feedback during the printing process (also present on the device screen). There are specific commands for manually feeding and cutting the tape, and a setup menu where the user can feed a new tape reel and calibrate the machine by adjust the letters alignment and pressure force applied to the daisy wheel.

### Embossing

The label production itself uses the same mechanical principles as the original machine, but is now ***reliable and automated***. A stepper motor feeds the tape while another selects the appropriate character on the daisy wheel according to a home position acquired by a hall sensor. Then a servo motor imprints each character by pressing it's daisy wheel tooth against the tape. That happens successively until the end of the desired label content, when there is a special character position to cut it off. A light blinks to ask for the label to be picked.

----

## Functional block diagram

<br>
<img width="100%" src="https://user-images.githubusercontent.com/15098003/196293832-6121e267-ee89-4fe7-bddf-c8d8f6bf96c8.png">
<p align="right">
  <a href ="https://user-images.githubusercontent.com/15098003/196293832-6121e267-ee89-4fe7-bddf-c8d8f6bf96c8.png" target="_blank">
    <i>🔎 see larger</i>
  </a>
</p>  
