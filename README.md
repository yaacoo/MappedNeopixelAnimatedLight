# Motivation
Building a smart animated decorative light. 

The idea was stuff an empty magnum Campari bottle with randomly placed RGB LEDs that can be set to a campari red color. Each LED on the strip (WS2812b) can be controlled individually with an arduino-like microcontroller (I used ESP8266 which is smaller and has wifi). 
Watch the [final result video](https://github.com/orr161/MappedNeopixelAnimatedLight/raw/main/exampleCampariLight.mp4).


# 2D Mapping (3D soon)
Since the LEDs are indexed from 0 to n-1 as they appear on the strip, I needed to map them to where they actually are located in space. I wrote a simple C code to light them on one at a time every 200 milliseconds:

```
uint32_t colorOn=strip.Color(70, 70, 70);
uint32_t colorOff=strip.Color(0, 0, 0);
  for (int i=0; i<LED_COUNT; i++) {
      strip.setPixelColor(i, colorOn);
      strip.setPixelColor(i-1, colorOff);
      strip.show();
      delay(200); // Wait 0.2 seconds between pixels
    }
```

I took a video in a dim room and processed in python (see  the notebook):

![Frame Example](https://raw.githubusercontent.com/orr161/MappedNeopixelAnimatedLight/main/Mapping.png)

# Animation
I loaded the coordinates into a simple C array, and made a simple animation that includes a drip of campary filling up the bottle and an animated sine wave function at the liquid-air interface. 

# OTA 
The C code includes default OTA blocks that allow pushing code updates over wifi (ESP8266 and other wifi enabled microcontrollers).
