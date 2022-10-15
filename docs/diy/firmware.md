---
layout: default
title: 💻 firmware
nav_order: 5
description: "E-TKT"
parent: 🛠️ do it yourself!
---

# 💻 **firmware**

1. Clone the [repository](https://github.com/andreisperid/E-TKT);
2. Make sure you have the [framework and all the libraries](https://andreisperid.github.io/E-TKT/credits/libraries.html) installed on your computer (I use [Visual Studio Code](https://code.visualstudio.com/) with [PlatformIO](https://platformio.org/) and recommend it!);
3. Flash the code into the ESP32 using an USB-C cable \*;
4. Upload the files ("data" folder) to the ESP32 using SPIFFS \*.

*\* don't forget to hold the \*FN button to enter DFU mode*

👀 if you want to know more about the code, please check the [firmware](https://github.com/andreisperid/E-TKT/blob/main/src/LabelMaker.cpp) and the [app scripts](https://github.com/andreisperid/E-TKT/blob/main/data/script.js).
