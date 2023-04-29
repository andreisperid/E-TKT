// OPTIONAL CONFIGURATION ---------------------------------------------------------

// Depending on the hardware you've used to build your E-TKT, you might need to enable some of these constants
// to get the hardware into a working state

// If your feed motor moves in the wrong direction by default, uncomment the define below to reverse it.
// It should be obvious if this is happening since the tape gets fed in the wrong direction.

// #define REVERSE_FEED_STEPPER_DIRECTION true

// If your hall sensor has inverted logic (eg active LOW and neutral HIGH) uncomment this #define to invert the
// logic checking it. If you're affected by this then you'll see the character carousel move forward slightly
// and then stop when the E-TKT starts up instead of moving to the "J" position.

// For instance, invert if using a "3144" hall sensor but don't invert if using a "44E 402" hall sensor.

// #define INVERT_HALL_SENSOR_LOGIC true

// If you're developing for the E-TKT it might be useful to enable OTA updates for the board firmware and filesystem.  
// Once firmware is uploaded with this option, you can use the env:ota-upload platformio target to upload to the device
// over wifi. Alternatively, you can visit http://e-tkt.local/update to upload binaries manually.
// #define OTA_ENABLED
