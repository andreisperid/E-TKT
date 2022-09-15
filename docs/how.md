---
layout: default
title: How does it work?
nav_order: 4
description: "E-TKT"
---

# ⚙️ how does it work?
![how](https://user-images.githubusercontent.com/15098003/171194737-37861a1f-fba7-404c-b987-5b3d26e704f3.png)

An *ESP32* commands the label production and also serves an on demand web application to any device connected in a local network.

***Neither internet is needed, nor installing any app.***

The whole process of connecting the E-TKT machine to a local network and then launching the app is aided by a small *OLED display* that provides instructions and a dynamically generated *QR code* with the *URL*, according to the IP attributed by the WLAN.

The *web app* provides text validation, special characters, a preview of the exact size of the tape, an option to select the desired lateral margins and also real-time feedback during the printing (also present on the device screen). There are also commands for attaching a new reel, manually feeding and cutting the tape.

The label production itself uses the same mechanical principles as the original machine did, but is now automated. A stepper motor feeds the tape while another selects the appropriate character on the carousel according to a home position acquired by hall sensor. Then a servo motor imprints each character by pressing the carousel to the tape. That happens successively until the end of the desired content, when there is a special character position to cut the label. A light blinks to ask for the label to be picked.
