# Smart Home Lipo Discharger 


This is quad Lipo Discharger with Apple Home Kit integration. No matter if you plug 3s, 4s or 6s lipos, the lipos will be discharged to ~25%-35% which translates in around 3.75V per cell. Once the discharge "percentage" is reached, a push message will be send to the phone. 

Why do you need a "smart home discharger" - no idea,- because it's winter time and you might want to build something during the exciting COVID19 quarantine?

![Alt text](https://github.com/moschotto/SmartHomeLipoDischarger/blob/main/media/Lipo_Discharger1_small.jpg)


## General information about the used HomeKit devices

Basiscally the code creates 9 Apple HomeKit deivces that can be paired with your HomeKit  homekit. YTH 9 devices? 

According to my understandig / research: 
1.  the only devices that can display 2 digit values which won't be round up to ".5" are temperature sensors
2.  the only devices that can display percentages are humidity sensors

For a quad port lipo discharger this means that for each lipo **one temperature** sensor and one **humidity sensor** is needed to display the voltage level and the discharge percentage. Additionally one temperature sensor for reading the surface temperature will be created. This sums up to **9 devices**.

Why is the percentage needed - isn't displaying the voltage enough?
Basically yes, but for displaying the status bar on the ESP32 display and for creating HomeKit automations, it's much easier to use the percentage instead of the voltage. Furthermore voltage levels differ between 3s,4s and 6s lipos.

## How to setup the ESP32

1. Wire the hardware as described in the wiring_diagram.pdf (do this at your own risk)
2. Upload the sketch / code to your ESP32.
3. Done

If you want to use of the HomeKit integration:

1. Open the COM/serial port via any terminal program or use the Arduino IDE Serial Monitor
2. Configure your WLAN settings via the "homespan" CLI. Homespan comes with it's own CLI which can be used via a regular serial interface. simply type in "W" in the terminal and follow the on-screen instructions. More information can be found here: https://github.com/HomeSpan/HomeSpan/blob/master/docs/CLI.md
3. check the boot process in the serial monitor (check for errors)
4. Open the HomeKit APP on your iPhone/iPad/Apple TV to pair the new device(s). Use my default pairing code "11112222" or the homespan default pairing code "46637726". The code can be changed in the setup routine.

Pairing process:

VIDEO here
  

## How to setup the HomeKit automation and push messages

blablablalb









## Libraries used:

Thanks for all the great projects libraries:


- HomeSpan -  for creating your own ESP32-based HomeKit devices
  https://github.com/HomeSpan/
  
- TFT_eSPI - for driving the display
  https://github.com/Bodmer/TFT_eSPI
  
- TJpg_Decoder - for startup animation / reading files from internal ESP32 memory (SPIFSS)
  https://github.com/Bodmer/TJpg_Decoder
  
- INA219_WE - voltage/current sensor
  https://github.com/wollewald/INA219_WE 

- Adafruit_BMP280 - for reading the temperature sensor
  https://github.com/adafruit/Adafruit_BMP280_Library



## Parts:
- 1 x ESP32 TTGO T-Display 
- 4 x INA219 current sensors
- 1 x BMP280 temperature sensor
- 1 x 4 channel 5V relay 
- 1 x  4.5-28V/3A DC-DC step down 
- 12 x G4 hallogen bulb sockets 
- 12 x G4 12V/20W hallogen bulbs 



