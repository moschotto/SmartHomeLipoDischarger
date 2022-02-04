# Smart Home Lipo Discharger 


This is quad Lipo Discharger with Apple Home Kit integration. No matter if you plug 3s, 4s or 6s lipos, the lipos will be discharged to ~25%-35% which translates in around 3.75V per cell. Once the discharge "percentage" is reached, a push message will be send to the phone. 


![Alt text](https://github.com/moschotto/SmartHomeLipoDischarger/blob/main/media/Lipo_Discharger1.jpg)


# Libraries used:

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


# Parts:
- 1 x TTGO T-Display $~14
- 4 x INA219 current sensor $~10
- 1 x 4CH relay $~2
- 1 x  4.5-28V/3A DC-DC step down $~2
- 12 x G4 hallogen bulb sockets $~8$
- 12 x G4 12V/20W hallogen bulbs $~5$



