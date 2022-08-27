# :battery: "Smart Home" Lipo Discharger 

This is quad Lipo Discharger with Apple Home Kit integration based on ESP32. No matter if you plug 3s, 4s or 6s lipos, the lipos will be discharged to ~25%-35% (around 3.75V per cell). Once the discharge "percentage" is reached, a push message will be send to the phone. Because halogen bulbs can get hot rapidly, i added a temperature sensor for checking the surface temperature - if the temperature reaches a certain level (e.g. 100C) the Discharger will shutdown. 

In my tests, the temperature was arround 70C after 15-20 minutes. Depending on the voltage and bulb watts (i used 3 x 20W per lipo), the discharge current is arround 1.1 - 1.4 amps.

 ### :exclamation:  Lipo safety: 
Do this at your own risk and read the getFPV.com lipo safety guide! 

https://www.getfpv.com/learn/fpv-essentials/drone-battery-safety/

If always charge and discharge lipo batteries outside, respectively in a fireproof environment i.e. outside


### :exclamation: CAUTION: 
If you are experimenting with different bulbs/watts, don't exceed 3A per lipo. The INA219 currentsensor is only rated up to 3.2A !!!
<br/>

![image_charger](https://github.com/moschotto/SmartHomeLipoDischarger/blob/main/media/Lipo_Discharger.jpg)

# Demo

https://www.youtube.com/watch?v=FhXnf4CWIPM

# How to setup the ESP32

1. Wire the hardware as described in the :electric_plug:[wiring diagram](https://github.com/moschotto/SmartHomeLipoDischarger/blob/main/wiring_diagram.pdf) (do this at your own risk)
2. Upload the sketch / code to your ESP32.
3. Extract the animation images from animation_pics.zip (see media folder), put it into the "data" folder and upload to SPIFSS via ARDUINO GUI

   [How to Upload data to ESP32](https://randomnerdtutorials.com/install-esp32-filesystem-uploader-arduino-ide/)
   
5. Done

## If you want to use the HomeKit integration:

1. Open the COM/serial port via any terminal program or use the Arduino IDE Serial Monitor
2. Configure your WLAN settings via the "homespan" CLI. Homespan comes with it's own CLI which can be used via a regular serial interface. simply type in "W" in the terminal and follow the on-screen instructions. More information can be found here: https://github.com/HomeSpan/HomeSpan/blob/master/docs/CLI.md
3. check the boot process in the serial monitor (check for errors)
4. Finaly open the HomeKit APP on your iPhone/iPad/Apple TV and pair the new device(s). Use my default pairing code "11112222" or the homespan default pairing code "46637726". The code can be changed in the setup routine.



Note:
If you delete the sensors within HomeKit, the discharger keeps its pairing information. This leads to problems when you want to re-add them (the device won't be discoverable anymore). Reset is possible via a browser or via serial interace or the homespan CLI. 

The simplest way is to type in the following URL:<br/> "IP address of your ESP32/reset" for example: 10.0.0.120/reset

After that, the device reboots and should be discoverable by HomeKit again.
   


# How to setup push messages and HomeKit automation

Because Apple HomeKit and siri shortcuts doesn't support push messages natively, you need to use a 3rd party app. Simple push messages can be easily generated by using "webhooks". For me IFTTT works perfectly fine,- with a free account you can use up to 4 "applets" but for the push messages you only need one. Simply download the APP, register, add the "webhook extension" and follow the instructions.


https://user-images.githubusercontent.com/10543292/152686966-74a350e1-0f22-4adf-b1d0-36e1d32a19c3.mp4

You must setup the HomeKit automation for each lipo in the same way. The only thing that needs to be changed during the automation setup is value1=4 to value1=1 for lipo 1, value1=2 for lipo2 etc etc.

Note that the URL contains **your personal key** (last section of the URL). This means that everyone that got your URL, can trigger push messages on your phone.


# General information about the used HomeKit devices

Basically the code creates 9 Apple HomeKit deivces that can be paired with your HomeKit. YTH 9 devices? 

According to my understandig / research: 
1.  the only devices that can display 2 digit values which won't be round up to ".5" are temperature sensors
2.  the only devices that can display percentages are humidity sensors

For a quad port lipo discharger this means for each lipo **1x temperature** sensor and **1x humidity sensor** is needed to display the voltage level AND the discharge percentage. Additionally one temperature sensor for reading the surface temperature will be created - makes 9 devices...

Why is the percentage used instead just the voltage?
For displaying the status bar on the ESP32 display and for creating HomeKit automations, it's much easier to use the percentage instead of the voltage. Because voltage levels differ between 3s,4s and 6s lipos you can simply automate it with: "if percentage drops below 20% then......."


# Libraries used:

Thanks for all the great projects / libraries:


- :green_book:HomeSpan -  for creating your own ESP32-based HomeKit devices<br/>
  https://github.com/HomeSpan/
  
- :green_book:TFT_eSPI - for driving the display<br/>
  https://github.com/Bodmer/TFT_eSPI
  
- :green_book:TJpg_Decoder - for startup animation / reading files from internal ESP32 memory (SPIFSS)<br/>
  https://github.com/Bodmer/TJpg_Decoder
  
- :green_book:INA219_WE - voltage/current sensor<br/>
  https://github.com/wollewald/INA219_WE 

- :green_book:Adafruit_BMP280 - for reading the temperature sensor<br/>
  https://github.com/adafruit/Adafruit_BMP280_Library



# Parts used:
- 1 x ESP32 TTGO T-Display 
- 4 x INA219 current sensors
- 1 x BMP280 temperature sensor
- 1 x 4 channel 5V relay 
- 1 x  4.5-28V/3A DC-DC step down 
- 12 x G4 hallogen bulb sockets 
- 12 x G4 12V/20W hallogen bulbs 

# Printable case / housing:
:computer: https://www.thingiverse.com/thing:5238418




[![Hits](https://hits.seeyoufarm.com/api/count/incr/badge.svg?url=https%3A%2F%2Fgithub.com%2Fmoschotto%2FSmartHomeLipoDischarger&count_bg=%2379C83D&title_bg=%23555555&icon=&icon_color=%23E7E7E7&title=hits&edge_flat=false)](https://hits.seeyoufarm.com)
