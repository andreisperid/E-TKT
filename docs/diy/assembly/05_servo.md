---
layout: default
title: 5. servo
description: "E-TKT"

parent: 🧩 assembly
grand_parent: 🛠️ do it yourself!
nav_order: 5
---

# **5. servo**

The servo, together with the hall sensor, is one of the most critical sub systems of the device.

***Please, be focused and follow thoroughly.***

![_DSC0539](https://user-images.githubusercontent.com/15098003/196191067-f6430e82-d6fd-4fb3-900d-c49b4bce222f.jpg)


## servo + PCB

![_DSC0540](https://user-images.githubusercontent.com/15098003/196191071-e32b1b19-2a8f-4e25-aeb5-bb78d8042977.jpg)

1. it is advisable to roll the excess cable with a wire strap to keep it tidy;
2. the correct connector order when facing the PCB is: GND (brown), VCC (red), SIGNAL (orange);
3. use an allen key to help push it to the header.


## servo: homing

Servos receive PWM signals to reach directly to a specific angle, in this case from 0 to 180 degrees.

It is unpredictable in which angle a given unit is delivered to you. Because of that, we'll be turning the machine on to make sure it is homed in the resting angle.

![_DSC0542](https://user-images.githubusercontent.com/15098003/196191073-89aa0f19-69f7-4820-b677-b6dd0658dd58.jpg)
![_DSC0545](https://user-images.githubusercontent.com/15098003/196191074-7be5f429-6352-4d60-9a2c-482f5ecead57.jpg)

1. make sure you have already flashed both the firmware and data files (SPIFFS) into the PCB;
2. connect the display to the PCB;
3. plug the 12DC power source into the PCB;
4. wait for the "E-TKT" splash animation and the servo will be homed to its resting position.

## P_press

![_DSC0547](https://user-images.githubusercontent.com/15098003/196191076-92adb8f3-eaf8-4014-92e5-d235145523cd.jpg)
![_DSC0548](https://user-images.githubusercontent.com/15098003/196191077-79708c40-8876-425b-9ef4-410c147d03ce.jpg)

1. place a m6 washer into the P_press;
2. this is intended to ease the movement between the P_press while keeping it perpendicular to the servo axis;


## P_press: alignment

Now that the servo is homed, we can put the P_press by using its own visual cue.

![_DSC0549](https://user-images.githubusercontent.com/15098003/196191080-0d023326-fb23-4046-8f0a-e71d8c568198.jpg)
![_DSC0550](https://user-images.githubusercontent.com/15098003/196191081-2885005a-32c6-43ef-bae0-169529123757.jpg)

1. don't forget to keep the device connected to the power source;
2. place the servo on a surface longitudinally;
3. use the lateral line of the P_press to keep it aligned;
4. aim the P_press hole to the servo hub and press gently until fitting it;
5. *do not twist, we need to "carve" the P_press plastic to the "cog" shape of the hub!*
6. place an m3x6 with washer and screw tightly
7. there must be absolutely no lateral play (force it a bit to test) between the servo and the P_press, otherwise it will miss the daisy wheel's tooth when forced. 


## + chassis

![_DSC0551](https://user-images.githubusercontent.com/15098003/196191086-55f37bf0-3e01-438f-8cf4-81cc9240379b.jpg)
![_DSC0556](https://user-images.githubusercontent.com/15098003/196191090-a3575acb-5dcf-4c34-aa14-71673e516052.jpg)
![_DSC0557](https://user-images.githubusercontent.com/15098003/196191092-e1553df8-05ed-432b-8d80-0f767adc43ce.jpg)
![_DSC0558](https://user-images.githubusercontent.com/15098003/196191093-8613eca9-5c9c-4054-abf0-034b65bfe1f8.jpg)

1. disconnect the power source from the PCB;
2. remove the display and reserve it;
3. to ease mounting the servo on the A_bottom, place 2x nuts and pass 2x m3x20 with washers;
4. screw both lightly, just to grab the nuts onto the other side;
5. keep a gap between the washers and the A_bottom;
6. align the servo to both screws and "click" it through;
7. tighten both screws while making sure the servo is as inward and pressed against to the A_bottom as possible;



## result & check

Just to make sure the P_press is aligned: with the machine turned off, twist the press a bit and turn the device on again.

***It should home to rest position. That means the lateral of the P_press should be 100% vertical, together with the servo outline.***

If not, you will need to print another P_press and repeat the previous steps.

![_DSC0559](https://user-images.githubusercontent.com/15098003/196191097-a4e6b799-653b-49f2-b412-7ccffab8f29c.jpg)
![_DSC0560](https://user-images.githubusercontent.com/15098003/196191099-62c4f33e-433f-4911-80b3-dccec5757463.jpg)
