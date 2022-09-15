---
layout: default
title: Home
nav_order: 1
description: "E-TKT"
permalink: /
---

<br>

![teaser_4](https://user-images.githubusercontent.com/15098003/171178685-e226b7ec-bc1c-44df-a134-f5b71171801a.gif)

<h1 align="center">
    E-TKT: anachronic label maker
</h1>

<p align="center">Andrei Speridião 2022 - http://andrei.cc</p>

<br>

# 🙃 what is it?
> ### *étiquette f (plural étiquettes)*
> ["ticket, memorandum, attach, stick, pierce, sting, to be sharp, goad, puncture, attach, nail"](https://en.wiktionary.org/wiki/%C3%A9tiquette#French)
> 1. tag, label 
> 2. prescribed behavior

<br>

E-TKT is a DIY label maker that mixes both old fashioned and contemporary technology to create something as simple as... Labels!

<br>

[![🎥 see it working 🎥](https://user-images.githubusercontent.com/15098003/171185500-8a63297c-487c-4900-b6d2-5c67298541d4.png)](https://www.youtube.com/watch?v=F0E5adLQ-AY "🎥 see it working 🎥")

<br>

# ⚙️ how does it work?
![how](https://user-images.githubusercontent.com/15098003/171194737-37861a1f-fba7-404c-b987-5b3d26e704f3.png)

An *ESP32* commands the label production and also serves an on demand web application to any device connected in a local network.

***Neither internet is needed, nor installing any app.***

The whole process of connecting the E-TKT machine to a local network and then launching the app is aided by a small *OLED display* that provides instructions and a dynamically generated *QR code* with the *URL*, according to the IP attributed by the WLAN.

The *web app* provides text validation, special characters, a preview of the exact size of the tape, an option to select the desired lateral margins and also real-time feedback during the printing (also present on the device screen). There are also commands for attaching a new reel, manually feeding and cutting the tape.

The label production itself uses the same mechanical principles as the original machine did, but is now automated. A stepper motor feeds the tape while another selects the appropriate character on the carousel according to a home position acquired by hall sensor. Then a servo motor imprints each character by pressing the carousel to the tape. That happens successively until the end of the desired content, when there is a special character position to cut the label. A light blinks to ask for the label to be picked.

<br>
