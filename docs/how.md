---
layout: default
title: ⚙️ how does it work?
nav_order: 4
description: "E-TKT"
---

# ⚙️ **how does it work?**

<img src="https://user-images.githubusercontent.com/15098003/190712733-7855c07e-cdea-4190-abf4-61ba5b3f6721.png">

An *ESP32* commands the label production and also serves an on demand web application (local) to any device connected in a local network.

***Neither internet is needed, nor installing any app.***

The whole process of connecting the E-TKT machine to a local network and then launching the local app is aided by a small *OLED display* that provides instructions and a dynamically generated *QR code* with the *URL*, according to the IP attributed by the WLAN.

The *web app* provides instant text validation, buttons for special characters, a preview of the physical size of the tape, an option to select the desired lateral margins and also real time feedback during the printing process (also present on the device screen). There are specific commands for manually feeding and cutting the tape, and setup menu where the user can feed a new reel and calibrate the machine by adjust the letters alignment and pressure force applied.

The label production itself uses the same mechanical principles as the original machine, but is now reliable and automated. A stepper motor feeds the tape while another selects the appropriate character on the daisy wheel according to a home position acquired by a hall sensor. Then a servo motor imprints each character by pressing it's daisy wheel tooth against the tape. That happens successively until the end of the desired label content, when there is a special character position to cut it off. A light blinks to ask for the label to be picked.

<br>

<img width="100%" src="https://user-images.githubusercontent.com/15098003/195631487-45085257-776d-4da8-94e5-62735814b732.png">
<p align="right">
  <a href ="https://user-images.githubusercontent.com/15098003/195631487-45085257-776d-4da8-94e5-62735814b732.png" target="_blank">
    <i>🔎 see larger</i>
  </a>
</p>  
