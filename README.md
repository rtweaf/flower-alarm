# flower-alarm
Control the temperature and humidity of the air around your flowers.

# Deps
* Software (Arduino SDK)
  - [LiquidCrystal_I2C](https://github.com/johnrickman/LiquidCrystal_I2C)
  - [DHT](https://github.com/adafruit/DHT-sensor-library)
  - [RtcDS1302](https://github.com/Makuna/Rtc)
* Hardware (RPI Pico)
  - LCD I2C
  - Logic level shifter
  - DHT11
  - Buzzer (without generator)
  - Tact switch (x2)
  - RTC DS1302
  
 :warning: In the macros, you can configure gpio pins.

# Todo
* [ ] Remember alarm ranges
* [ ] Soil humidity monitoring
