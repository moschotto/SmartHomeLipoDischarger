//Lipo Discahrger 1.0
//
//Copyright (C) by mosch
//License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
//GITHUB: https://github.com/moschotto/SmartHomeLipoDischarger
/////////////////////////////////////////////////////////////////////

#define FS_NO_GLOBALS
#include <FS.h>
#include "SPIFFS.h"

#include "HomeSpan.h"
#include "DEV_batt_sernsor.h"
#include "HAP.h"
#include <nvs_flash.h>
#include <WebServer.h>

#include <Button2.h>
#include <Wire.h>
#include <INA219_WE.h>
#include <SPI.h>
#include <TJpg_Decoder.h>
#include <TFT_eSPI.h>

#include <Adafruit_BMP280.h>

WebServer webServer(80);
///////////////////////////////////////////
//define relay PINs and buttons
#define relay_pin1 12
#define relay_pin2 13
#define relay_pin3 15
#define relay_pin4 2

#define BUTTON_1        35
#define BUTTON_2        0

Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);

///////////////////////////////////////////
//define current sensors
#define I2C_ADDRESS1 0x40
#define I2C_ADDRESS2 0x41
#define I2C_ADDRESS3 0x44
#define I2C_ADDRESS4 0x45

INA219_WE ina219_1 = INA219_WE(I2C_ADDRESS1);
INA219_WE ina219_2 = INA219_WE(I2C_ADDRESS2);
INA219_WE ina219_3 = INA219_WE(I2C_ADDRESS3);
INA219_WE ina219_4 = INA219_WE(I2C_ADDRESS4);

float ina219_1_correctionFactor = 0.96;
float ina219_2_correctionFactor = 0.96;
float ina219_3_correctionFactor = 0.96;
float ina219_4_correctionFactor = 0.96;

///////////////////////////////////////////
//define temperature sensor
Adafruit_BMP280 bmp;
#define BMP280_I2C_ADDRESS  0x76

///////////////////////////////////////////
//define display
#ifndef TFT_DISPOFF
#define TFT_DISPOFF 0x25
#endif

#ifndef TFT_SLPIN
#define TFT_SLPIN   0x10
#endif

#define TFT_MOSI            19
#define TFT_SCLK            18
#define TFT_CS              5
#define TFT_DC              16
#define TFT_RST             23

#define TFT_BL          4   // Display backlight control pin
#define ADC_EN          14  //ADC_EN is the ADC detection enable port
#define ADC_PIN         34

#define RED2RED     0
#define GREEN2GREEN 1
#define BLUE2BLUE   2
#define BLUE2RED    3
#define GREEN2RED   4
#define RED2GREEN   5
#define RAINBOW     6

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library
TFT_eSprite img = TFT_eSprite(&tft);

///////////////////////////////////////////
//define global variables

float shuntVoltage_mV = 0.0;
float busVoltage_V = 0.0;
float power_mW = 0.0;
bool ina219_overflow = false;

float loadVoltage1_V = 0.0;
float current1_mA = 0.0;
float loadVoltage2_V = 0.0;
float current2_mA = 0.0;
float loadVoltage3_V = 0.0;
float current3_mA = 0.0;
float loadVoltage4_V = 0.0;
float current4_mA = 0.0;

int lipo1_percentage = 0;
int lipo2_percentage = 0;
int lipo3_percentage = 0;
int lipo4_percentage = 0;
int lipo_discharge_percentage = 15;  // note: after discharging process the lipo most likely recovers to ~35-40%
int lipo_discharge_percentage_tolerance = 10 ;

float voltage = 0.0;
float voltage_tmp = 0.0;
float lipo_percentage = 0;
boolean lipo1_power_off_flag = false;
boolean lipo2_power_off_flag = false;
boolean lipo3_power_off_flag = false;
boolean lipo4_power_off_flag = false;

//lipo percentage mapping
float arr_percent_3s[21][2] = {{0,9.82},{5,10.83},{10,11.06},{15,11.12},{20,11.18},{25,11.24},{30,11.30},{35,11.36},{40,11.39},{45,11.45},{50,11.51},{55,11.56},{60,11.62},{65,11.74},{70,11.86},{75,11.95},{80,12.07},{85,12.25},{90,12.33},{95,12.45},{100,12.60}};
float arr_percent_4s[21][2] = {{0,13.09},{5,14.43},{10,14.75},{15,14.83},{20,14.91},{25,14.99},{30,15.06},{35,15.14},{40,15.18},{45,15.26},{50,15.34},{55,15.42},{60,15.50},{65,15.66},{70,15.81},{75,15.93},{80,16.09},{85,16.33},{90,16.45},{95,16.60},{100,16.80}};
float arr_percent_6s[21][2] = {{0,19.64},{5,21.65},{10,22.12},{15,22.24},{20,22.36},{25,22.48},{30,22.60},{35,22.72},{40,22.77},{45,22.89},{50,23.01},{55,23.13},{60,23.25},{65,23.48},{70,23.72},{75,23.90},{80,24.14},{85,24.49},{90,24.67},{95,24.90},{100,25.20}};
int battery_cells = 0;

//define update mainloop_interval
unsigned long mainloop_previousMillis = 0;
unsigned long mainloop_currentMillis = 0;
const long mainloop_interval = 5000;

//define temperature measurment interval
unsigned long temperature_previousMillis = 0;
unsigned long temperature_currentMillis = 0;
const long temperature_interval = 14241; // update interval of the temperature (should be not devidable by main_loopinterval)

//define filenames of images in SPIFSS (for startup animation )
String frame_prefix = "/frame";
String frame_name = "";
String frame_suffix = ".jpg";
int imgNum = 0;


///////////////////////////////////////////
//define functions

void init_ina219_sensors();
void reinit_ina219_sensors();
void button_init();
void update_lipo(int lipo_number);
void get_lipo_cells(float voltage);
void get_lipo_percentage(float voltage);
void get_INA219_data();
void linearMeter(int val, int x, int y, int w, int h, int g, int n, byte s, bool horizontial);
uint16_t rainbowColor(uint8_t spectrum);
void update_display();
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap);
void startup_animation();
void setupWeb();


void setup() {

  ///////////////////////////////////////////
  //init display / and JPG Decoder
  ///////////////////////////////////////////
  pinMode(ADC_EN, OUTPUT);
  digitalWrite(ADC_EN, HIGH);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  img.createSprite(240, 135);

  TJpgDec.setJpgScale(1);
  TJpgDec.setSwapBytes(true);
  TJpgDec.setCallback(tft_output);

  ///////////////////////////////////////////
  //general init
  ///////////////////////////////////////////
  Serial.begin(115200);
  Wire.begin(21,22);

  button_init(); //ESP32 button handling routine
  pinMode(relay_pin1, OUTPUT);
  pinMode(relay_pin2, OUTPUT);
  pinMode(relay_pin3, OUTPUT);
  pinMode(relay_pin4, OUTPUT);

  //init relay -> HIGH=relay open, LOW=relay closed
  digitalWrite(relay_pin1, HIGH);
  digitalWrite(relay_pin2, HIGH);
  digitalWrite(relay_pin3, HIGH);
  digitalWrite(relay_pin4, HIGH);

  ///////////////////////////////////////////
  //init SPIFFS
  ///////////////////////////////////////////
  if (!SPIFFS.begin()) {
   img.drawString("SPIFSS Initialisation FAILED...." , 10 ,50,2);
   Serial.println("SPIFSS Initialisation FAILED....");
   //while (1) yield(); // Stay here twiddling thumbs waiting
  }
  else
  {
    img.drawString("SPIFSS Initialisation done...." , 10 ,50,2);
  }
  ///////////////////////////////////////////
  //init current sensor
  ///////////////////////////////////////////
  bmp.begin(BMP280_I2C_ADDRESS);

  ///////////////////////////////////////////
  //init current sensors
  ///////////////////////////////////////////
  init_ina219_sensors();

  ///////////////////////////////////////////
  //init homespan / homekit and webserver
  ///////////////////////////////////////////
  {
  homeSpan.setHostNameSuffix("");         // use null string for suffix (rather than the HomeSpan device ID)
  homeSpan.setPortNum(1201);              // change port number for HomeSpan so we can use port 80 for the Web Server
  homeSpan.setMaxConnections(5);          // reduce max connection to 5 (default is 8) since WebServer and a connecting client will need 2, and OTA needs 1
  homeSpan.setWifiCallback(setupWeb);     // need to start Web Server after WiFi is established
  homeSpan.enableOTA();                   // default password homespan-ota
  homeSpan.begin(Category::Bridges,"Lipo Discharger2");
  homeSpan.setPairingCode("11112222");    // define HomeKit pairing pin

  //define HomeKit devices
  new SpanAccessory();
      new Service::AccessoryInformation();
      new Characteristic::Name("Lipo Discharger temperature");
      new Characteristic::Manufacturer("Mosch");
      new Characteristic::SerialNumber("000-000");
      new Characteristic::Model("ESP Lipo Discharger");
      new Characteristic::FirmwareRevision("1.0");
      new Characteristic::Identify();
      new Service::HAPProtocolInformation();
      new Characteristic::Version("1.1.0");
      new DEV_Discharger_temp();

  new SpanAccessory();
      new Service::AccessoryInformation();
      new Characteristic::Name("Lipo 1 Voltage");
      new Characteristic::Manufacturer("Mosch");
      new Characteristic::SerialNumber("000-001");
      new Characteristic::Model("ESP Lipo Discharger");
      new Characteristic::FirmwareRevision("1.0");
      new Characteristic::Identify();
      new Service::HAPProtocolInformation();
      new Characteristic::Version("1.1.0");
      new DEV_Lipo1_temp();

  new SpanAccessory();
      new Service::AccessoryInformation();
      new Characteristic::Name("Lipo 2 Voltage");
      new Characteristic::Manufacturer("Mosch");
      new Characteristic::SerialNumber("000-002");
      new Characteristic::Model("ESP Lipo Discharger");
      new Characteristic::FirmwareRevision("1.0");
      new Characteristic::Identify();
      new Service::HAPProtocolInformation();
      new Characteristic::Version("1.1.0");
      new DEV_Lipo2_temp();

  new SpanAccessory();
      new Service::AccessoryInformation();
      new Characteristic::Name("Lipo 3 Voltage");
      new Characteristic::Manufacturer("Mosch");
      new Characteristic::SerialNumber("000-003");
      new Characteristic::Model("ESP Lipo Discharger");
      new Characteristic::FirmwareRevision("1.0");
      new Characteristic::Identify();
      new Service::HAPProtocolInformation();
      new Characteristic::Version("1.1.0");
      new DEV_Lipo3_temp();

  new SpanAccessory();
      new Service::AccessoryInformation();
      new Characteristic::Name("Lipo 4 Voltage");
      new Characteristic::Manufacturer("Mosch");
      new Characteristic::SerialNumber("000-004");
      new Characteristic::Model("ESP Lipo Discharger");
      new Characteristic::FirmwareRevision("1.0");
      new Characteristic::Identify();
      new Service::HAPProtocolInformation();
      new Characteristic::Version("1.1.0");
      new DEV_Lipo4_temp();

  ///////////////////////////////////////////////////////////////////////

  new SpanAccessory();
      new Service::AccessoryInformation();
      new Characteristic::Name("Lipo 1 Percentage");
      new Characteristic::Manufacturer("Mosch");
      new Characteristic::SerialNumber("000-005");
      new Characteristic::Model("ESP Lipo Discharger");
      new Characteristic::FirmwareRevision("1.0");
      new Characteristic::Identify();
      new Service::HAPProtocolInformation();
      new Characteristic::Version("1.1.0");
      new DEV_Lipo1_hum();

  new SpanAccessory();
      new Service::AccessoryInformation();
      new Characteristic::Name("Lipo 2 Percentage");
      new Characteristic::Manufacturer("Mosch");
      new Characteristic::SerialNumber("000-006");
      new Characteristic::Model("ESP Lipo Discharger");
      new Characteristic::FirmwareRevision("1.0");
      new Characteristic::Identify();
      new Service::HAPProtocolInformation();
      new Characteristic::Version("1.1.0");
      new DEV_Lipo2_hum();

  new SpanAccessory();
      new Service::AccessoryInformation();
      new Characteristic::Name("Lipo 3 Percentage");
      new Characteristic::Manufacturer("Mosch");
      new Characteristic::SerialNumber("000-007");
      new Characteristic::Model("ESP Lipo Discharger");
      new Characteristic::FirmwareRevision("1.0");
      new Characteristic::Identify();
      new Service::HAPProtocolInformation();
      new Characteristic::Version("1.1.0");
      new DEV_Lipo3_hum();

  new SpanAccessory();
      new Service::AccessoryInformation();
      new Characteristic::Name("Lipo 4 Percentage");
      new Characteristic::Manufacturer("Mosch");
      new Characteristic::SerialNumber("000-008");
      new Characteristic::Model("ESP Lipo Discharger");
      new Characteristic::FirmwareRevision("1.0");
      new Characteristic::Identify();
      new Service::HAPProtocolInformation();
      new Characteristic::Version("1.1.0");
      new DEV_Lipo4_hum();
}

startup_animation();

}

////////////////////////////////////////////////////////////////////////////////
//main loop
////////////////////////////////////////////////////////////////////////////////

void loop(){

  btn1.loop();
  btn2.loop();
  homeSpan.poll();
  webServer.handleClient();

  //data update after X seconds
  mainloop_currentMillis = millis();
  if (mainloop_currentMillis - mainloop_previousMillis >= mainloop_interval)
  {

    //get sensor data (voltage/current)
    get_INA219_data();

    //update display
    img.fillSprite(TFT_BLACK);
    img.drawFastHLine(0,0,240, TFT_WHITE);
    img.drawFastVLine(0,0,135, TFT_WHITE);
    img.drawFastVLine(239,0,135, TFT_WHITE);
    img.drawFastHLine(0,134,240, TFT_WHITE);

    update_lipo(1);
    homeSpan.poll();
    delay(500);

    update_lipo(2);
    homeSpan.poll();
    delay(500);

    update_lipo(3);
    homeSpan.poll();
    delay(500);

    update_lipo(4);
    homeSpan.poll();
    delay(500);
    update_display();

    mainloop_previousMillis = mainloop_currentMillis;
  }

  //display temperature every X seconds
  temperature_currentMillis = millis();
  if (temperature_currentMillis - temperature_previousMillis >= temperature_interval)
  {
    float discharger_temperature = bmp.readTemperature();

    Discharger_temperature->setVal(discharger_temperature);

    if(discharger_temperature > 100.0)
    {
      img.setTextColor(TFT_RED);
      img.fillRoundRect(40, 15, 170, 75 , 8, TFT_DARKGREY);
      img.drawString("Temperature: ", 50 ,20,4);
      img.drawString(String(discharger_temperature) + "°C", 80 ,60,4);

      //power off relay
      digitalWrite(relay_pin1, HIGH);
      digitalWrite(relay_pin2, HIGH);
      digitalWrite(relay_pin3, HIGH);
      digitalWrite(relay_pin4, HIGH);
    }
    else
    {
      img.setTextColor(TFT_WHITE);
      img.fillRoundRect(70, 55, 100, 30 , 8, TFT_DARKGREY);
      img.drawString(String(discharger_temperature) + "°C", 80 ,60,4);
    }

    img.pushSprite(0, 0);
    temperature_previousMillis = temperature_currentMillis;
    img.setTextColor(TFT_WHITE);
  }
}

////////////////////////////////////////////////////////////////////////////////
//functions
////////////////////////////////////////////////////////////////////////////////

void button_init()
{
  btn1.setPressedHandler([](Button2 & b) {

  // startup_animation();
  });

  btn2.setPressedHandler([](Button2 & b) {

  });
}
void update_lipo(int lipo_number)
{
  switch (lipo_number) {
  case 1:
    Serial.println("Updating Lipo 1");

    get_lipo_cells(loadVoltage1_V);
    get_lipo_percentage(loadVoltage1_V);

    voltage_tmp = loadVoltage1_V;
    lipo1_percentage = lipo_percentage; //for update display function

    if(voltage_tmp < 10.0)  { voltage_tmp = 0.0; lipo1_power_off_flag = false;digitalWrite(relay_pin1, HIGH);}
    //if(lipo1_percentage > lipo_discharge_percentage) { power_off_flag = false;}


    if(voltage_tmp >= 0.0 && voltage_tmp < 101.0)
    {
      Lipo1_Voltage->setVal(voltage_tmp);
    }
    if(lipo1_percentage >= 0 && lipo1_percentage < 101)
    {
      Lipo1_hum_Percentage->setVal(lipo1_percentage);
    }

    //power on relay
    if(lipo1_percentage > 0 && lipo1_percentage > lipo_discharge_percentage && lipo1_power_off_flag == false)
     {
       Lipo1_StatusLowBattery->setVal(0);
       digitalWrite(relay_pin1, LOW);

       //reinit sesnor after (re)power on relay.
       //this seems to be neccessary in oder to measure the current (voltage measurement work as expected)
       reinit_ina219_sensors();
     }

     //power off relay
    if(lipo1_percentage > 0 && lipo1_percentage <= lipo_discharge_percentage)
    {
       Lipo1_StatusLowBattery->setVal(1);
       digitalWrite(relay_pin1, HIGH);
       lipo1_power_off_flag = true;
    }

    break;

  case 2:
    Serial.println("Updating Lipo 2");

    get_lipo_cells(loadVoltage2_V);
    get_lipo_percentage(loadVoltage2_V);

    voltage_tmp = loadVoltage2_V;
    lipo2_percentage = lipo_percentage;

    if(voltage_tmp < 10.0)  { voltage_tmp = 0.0; lipo2_power_off_flag = false; digitalWrite(relay_pin2, HIGH);}

    if(voltage_tmp >= 0.0 && voltage_tmp < 101.0)
    {
      Lipo2_Voltage->setVal(voltage_tmp);
    }
    if(lipo2_percentage >= 0 && lipo2_percentage < 101)
    {
      Lipo2_hum_Percentage->setVal(lipo2_percentage);
    }

    if(lipo2_percentage > 0 && lipo2_percentage > lipo_discharge_percentage && lipo2_power_off_flag == false)
    {
      Lipo2_StatusLowBattery->setVal(0);
      digitalWrite(relay_pin2, LOW);

      //reinit sesnor after (re)power on relay.
      //this seems to be neccessary in oder to measure the current (voltage measurement work as expected)
      reinit_ina219_sensors();
    }

    if(lipo2_percentage > 0 && lipo2_percentage <= lipo_discharge_percentage)
    {
       Lipo2_StatusLowBattery->setVal(1);
      digitalWrite(relay_pin2, HIGH);
       lipo2_power_off_flag = true;
    }

    break;

  case 3:
    Serial.println("Updating Lipo 3");

    get_lipo_cells(loadVoltage3_V);
    get_lipo_percentage(loadVoltage3_V);

    voltage_tmp = loadVoltage3_V;
    lipo3_percentage = lipo_percentage;

    if(voltage_tmp < 1.0)  { voltage_tmp = 0.0; lipo3_power_off_flag = false; digitalWrite(relay_pin3, HIGH);}

    if(voltage_tmp >= 0.0 && voltage_tmp < 101.0)
    {
      Lipo3_Voltage->setVal(voltage_tmp);
    }
    if(lipo3_percentage >= 0 && lipo3_percentage < 101)
    {
      Lipo3_hum_Percentage->setVal(lipo3_percentage);
    }
    if(lipo3_percentage > 0 && lipo3_percentage > lipo_discharge_percentage && lipo3_power_off_flag == false)
    {
      Lipo3_StatusLowBattery->setVal(0);
      digitalWrite(relay_pin3, LOW);

      //reinit sesnor after (re)power on relay.
      //this seems to be neccessary in oder to measure the current (voltage measurement work as expected)
      reinit_ina219_sensors();
    }
    if(lipo3_percentage > 0 && lipo3_percentage <= lipo_discharge_percentage)
    {
       Lipo3_StatusLowBattery->setVal(1);
       digitalWrite(relay_pin3, HIGH);
       lipo3_power_off_flag = true;
    }
    break;

  case 4:
    Serial.println("Updating Lipo 4");

    get_lipo_cells(loadVoltage4_V);
    get_lipo_percentage(loadVoltage4_V);

    voltage_tmp = loadVoltage4_V;
    lipo4_percentage = lipo_percentage;

    if(voltage_tmp < 1.0)  { voltage_tmp = 0.0; lipo4_power_off_flag = false; digitalWrite(relay_pin4, HIGH);}

    if(voltage_tmp >= 0.0 && voltage_tmp < 101.0)
    {
      Lipo4_Voltage->setVal(voltage_tmp);
    }
    if(lipo4_percentage >= 0 && lipo4_percentage < 101)
    {
      Lipo4_hum_Percentage->setVal(lipo4_percentage);
    }
    if(lipo4_percentage > 0 && lipo4_percentage > lipo_discharge_percentage && lipo4_power_off_flag == false)
    {
      Lipo4_StatusLowBattery->setVal(0);
      digitalWrite(relay_pin4, LOW);

      //reinit sesnor after (re)power on relay.
      //this seems to be neccessary in oder to measure the current (voltage measurement work as expected)
      reinit_ina219_sensors();
    }
    if(lipo4_percentage > 0 && lipo4_percentage <= lipo_discharge_percentage)
    {
       Lipo4_StatusLowBattery->setVal(1);
       digitalWrite(relay_pin4, HIGH);
       lipo4_power_off_flag = true;
    }
    break;
  default:

    break;
  }
}
void update_display()
{

  int tmp_percent = 0;
  int spacing = 59; //spacing between LIPOs
  int x_pos = 12; //start positon of the first LIPO

  ////////////////////////////////////////////////////
  //LIPO 1

  if(loadVoltage1_V < 1.0)  { loadVoltage1_V = 0.0;  }

  tmp_percent =  map(lipo1_percentage, 0, 100, 0, 10);
  img.drawString(String(loadVoltage1_V) + " V", x_pos ,5, 2);
  img.drawString(String(ina219_1.getCurrent_mA()/1000.0) + " A", x_pos , 20, 2);

  linearMeter(tmp_percent,  x_pos, 105, 40, 4, 1, 10, RED2GREEN,false);
  img.drawRoundRect(x_pos + 10, 40, 20, 10 , 3, TFT_WHITE);
  img.drawRoundRect(x_pos - 5, 50, 50, 75 , 3, TFT_WHITE);

  img.drawString(String(lipo1_percentage) + "%", x_pos + 8 , 107, 2);

  ////////////////////////////////////////////////////
  //LIPO 2

  if(loadVoltage2_V < 1.0)  { loadVoltage2_V = 0.0;  }

  tmp_percent =  map(lipo2_percentage, 0, 100, 0, 10);
  img.drawString(String(loadVoltage2_V) + " V", x_pos + spacing  ,5, 2);
  img.drawString(String(ina219_2.getCurrent_mA()/1000.0) + " A", x_pos + spacing , 20, 2);

  linearMeter(tmp_percent,  x_pos + spacing, 105, 40, 4, 1, 10, RED2GREEN,false);
  img.drawRoundRect(x_pos + 10 + spacing, 40, 20, 10 , 3, TFT_WHITE);
  img.drawRoundRect((x_pos - 5) + spacing, 50, 50, 75 , 3, TFT_WHITE);

  img.drawString(String(lipo2_percentage) + "%", x_pos + 8 + spacing, 107, 2);

  ////////////////////////////////////////////////////
  //LIPO 3

  if(loadVoltage3_V < 1.0)  { loadVoltage3_V = 0.0;  }

  tmp_percent =  map(lipo3_percentage, 0, 100, 0, 10);
  img.drawString(String(loadVoltage3_V) + " V", x_pos + (spacing*2) ,5, 2);
  img.drawString(String(ina219_3.getCurrent_mA()/1000.0) + " A", x_pos + (spacing*2) , 20, 2);

  linearMeter(tmp_percent,  x_pos + (spacing *2), 105, 40, 4, 1, 10, RED2GREEN,false);
  img.drawRoundRect(x_pos + 10 + (spacing *2), 40, 20, 10 , 3, TFT_WHITE);
  img.drawRoundRect((x_pos - 5) + (spacing *2), 50, 50, 75 , 3, TFT_WHITE);

  img.drawString(String(lipo3_percentage) + "%", x_pos + 8 + (spacing*2), 107, 2);

  ////////////////////////////////////////////////////
  //LIPO 4

  if(loadVoltage4_V < 1.0)  { loadVoltage4_V = 0.0;  }

  tmp_percent =  map(lipo4_percentage, 0, 100, 0, 10);
  img.drawString(String(loadVoltage4_V) + " V", x_pos + (spacing*3) ,5, 2);
  img.drawString(String(ina219_4.getCurrent_mA()/1000.0) + " A", x_pos + (spacing*3) , 20, 2);

  linearMeter(tmp_percent,  x_pos + (spacing *3), 105, 40, 4, 1, 10, RED2GREEN,false);
  img.drawRoundRect(x_pos + 10 + (spacing *3), 40, 20, 10 , 3, TFT_WHITE);
  img.drawRoundRect((x_pos - 5) + (spacing *3), 50, 50, 75 , 3, TFT_WHITE);

  img.drawString(String(lipo4_percentage) + "%", x_pos + 8 + (spacing *3), 107, 2);

  img.pushSprite(0, 0);

}
void get_lipo_cells(float voltage)
{

  battery_cells = 0.0;

  //check LIPO type
  if(voltage >= 9.9 &&  voltage < 12.6 + 0.2)
  {
    battery_cells = 3.0;
  }
  if(voltage >= 13.2 &&  voltage < 16.8 + 0.2)
  {
    battery_cells = 4.0;
  }
  if(voltage >= 19.8 &&  voltage <= 25.2 + 0.2)
  {
    battery_cells = 6.0;
  }
}
void get_lipo_percentage(float voltage)
{
  lipo_percentage = 0;

  if(battery_cells == 4.0)
  {
    for(int i=0;i<=20;i++)
    {
      if(voltage > arr_percent_4s[i][1])
      {
        lipo_percentage = arr_percent_4s[i][0];
      }
    }
  }

  if(battery_cells == 6.0)
  {
    for(int i=0;i<=10;i++)
    {
      if(voltage > arr_percent_6s[i][1])
      {
        lipo_percentage = arr_percent_6s[i][0];
      }
    }
  }

}
void get_INA219_data()
{

  float busVoltage1_V = ina219_1.getBusVoltage_V();
  float shuntVoltage1_mV = ina219_1.getShuntVoltage_mV();
  current1_mA = ina219_1.getCurrent_mA();
  loadVoltage1_V  = busVoltage1_V + (shuntVoltage1_mV/1000);

  float busVoltage2_V = ina219_2.getBusVoltage_V();
  float shuntVoltage2_mV = ina219_2.getShuntVoltage_mV();
  current2_mA = ina219_2.getCurrent_mA();
  loadVoltage2_V  = busVoltage2_V + (shuntVoltage2_mV/1000);

  float busVoltage3_V = ina219_3.getBusVoltage_V();
  float shuntVoltage3_mV = ina219_3.getShuntVoltage_mV();
  current3_mA = ina219_3.getCurrent_mA();
  loadVoltage3_V  = busVoltage3_V + (shuntVoltage3_mV/1000);

  float busVoltage4_V = ina219_4.getBusVoltage_V();
  float shuntVoltage4_mV = ina219_4.getShuntVoltage_mV();
  current4_mA = ina219_4.getCurrent_mA();
  loadVoltage4_V  = busVoltage4_V + (shuntVoltage4_mV/1000);

}
void linearMeter(int val, int x, int y, int w, int h, int g, int n, byte s, bool horizontial)
{
  // #########################################################################
  //  Draw the linear meter
  // #########################################################################
  // val =  reading to show (range is 0 to n)
  // x, y = position of top left corner  (if horizontial)
  // x, y = bottom left corner  (if horizontial)
  // w, h = width and height of a single bar
  // g    = pixel gap to next bar (can be 0)
  // n    = number of segments
  // s    = colour scheme

  // Variable to save "value" text colour from scheme and set default
  int colour = TFT_BLUE;
  // Draw n colour blocks
  for (int b = 1; b <= n; b++) {
    if (val > 0 && b <= val) { // Fill in coloured blocks
      switch (s) {
        case 0: colour = TFT_RED; break; // Fixed colour
        case 1: colour = TFT_GREEN; break; // Fixed colour
        case 2: colour = TFT_BLUE; break; // Fixed colour
        case 3: colour = rainbowColor(map(b, 0, n, 127,   0)); break; // Blue to red
        case 4: colour = rainbowColor(map(b, 0, n,  63,   0)); break; // Green to red
        case 5: colour = rainbowColor(map(b, 0, n,   0,  63)); break; // Red to green
        case 6: colour = rainbowColor(map(b, 0, n,   0, 159)); break; // Rainbow (red to violet)
      }
      if(horizontial)
      {
        img.fillRect(x + b*(w+g), y, w, h, colour);
      }
      else
      {
        img.fillRect(x , y - b*(h+g), w, h, colour);
      }


    }
    else // Fill in blank segments
    {
      if(horizontial)
      {
        img.fillRect(x + b*(w+g), y, w, h, TFT_DARKGREY);
      }
      else
      {
        img.fillRect(x, y - b*(h+g), w, h, TFT_DARKGREY);
      }

    }
  }

}
uint16_t rainbowColor(uint8_t spectrum)
{

  /***************************************************************************************
  ** Function name:           rainbowColor
  ** Description:             Return a 16 bit rainbow colour
  ***************************************************************************************/
    // If 'spectrum' is in the range 0-159 it is converted to a spectrum colour
    // from 0 = red through to 127 = blue to 159 = violet
    // Extending the range to 0-191 adds a further violet to red band


  spectrum = spectrum%192;

  uint8_t red   = 0; // Red is the top 5 bits of a 16 bit colour spectrum
  uint8_t green = 0; // Green is the middle 6 bits, but only top 5 bits used here
  uint8_t blue  = 0; // Blue is the bottom 5 bits

  uint8_t sector = spectrum >> 5;
  uint8_t amplit = spectrum & 0x1F;

  switch (sector)
  {
    case 0:
      red   = 0x1F;
      green = amplit; // Green ramps up
      blue  = 0;
      break;
    case 1:
      red   = 0x1F - amplit; // Red ramps down
      green = 0x1F;
      blue  = 0;
      break;
    case 2:
      red   = 0;
      green = 0x1F;
      blue  = amplit; // Blue ramps up
      break;
    case 3:
      red   = 0;
      green = 0x1F - amplit; // Green ramps down
      blue  = 0x1F;
      break;
    case 4:
      red   = amplit; // Red ramps up
      green = 0;
      blue  = 0x1F;
      break;
    case 5:
      red   = 0x1F;
      green = 0;
      blue  = 0x1F - amplit; // Blue ramps down
      break;
  }

  return red << 11 | green << 6 | blue;
}
void startup_animation()
{
  int frame_counter = 0;

  img.fillSprite(TFT_BLACK);
  img.pushSprite(0, 0);

  TJpgDec.setJpgScale(1);

  File root = SPIFFS.open("/");
  File file = root.openNextFile();

  //get number of files
  while(file)
  {
      frame_counter++;
      //TJpgDec.drawFsJpg(0, 0, file.name());
      file = root.openNextFile();
  }

  //play startup animation
  for(int i=0;i<frame_counter;i++)
  {

    //image name
    frame_name = frame_prefix + String(i) + frame_suffix;

    if(SPIFFS.exists(frame_name))
    {
      TJpgDec.drawFsJpg(0, 0, frame_name);
    }
    else
    {

      img.setTextColor(TFT_RED);
      img.fillSprite(TFT_BLACK);
      img.drawString("Can't load image: ", 10 ,35,4);
      img.drawString(frame_name , 10 ,70,4);
      img.pushSprite(0, 0);
      delay(1);
    }
  }
  img.setTextColor(TFT_WHITE);


}
void init_ina219_sensors()
{
  if(!ina219_1.init()){
    Serial.println("INA219_1 not connected!");
  }
  if(!ina219_2.init()){
    Serial.println("INA219_2 not connected!");
  }
  if(!ina219_3.init()){
    Serial.println("INA219_3 not connected!");
  }
  if(!ina219_4.init()){
    Serial.println("INA219_4 not connected!");
  }
  ina219_1.setBusRange(BRNG_32); // set voltage range to 32V
  ina219_1.setCorrectionFactor(ina219_1_correctionFactor); // set correction factor accordingly -> measure the current / voltage with a multimeter
  ina219_2.setBusRange(BRNG_32); // set voltage range to 32V
  ina219_2.setCorrectionFactor(ina219_2_correctionFactor); // set correction factor accordingly -> measure the current / voltage with a multimeter
  ina219_3.setBusRange(BRNG_32); // set voltage range to 32V
  ina219_3.setCorrectionFactor(ina219_3_correctionFactor); // set correction factor accordingly -> measure the current / voltage with a multimeter
  ina219_4.setBusRange(BRNG_32); // set voltage range to 32V
  ina219_4.setCorrectionFactor(ina219_4_correctionFactor); // set correction factor accordingly -> measure the current / voltage with a multimeter

  delay(1000);
}
void reinit_ina219_sensors()
{
  ina219_1.reset_INA219();
  ina219_1.init();
  ina219_1.setBusRange(BRNG_32);
  ina219_1.setCorrectionFactor(ina219_1_correctionFactor);

  ina219_2.reset_INA219();
  ina219_2.init();
  ina219_2.setBusRange(BRNG_32);
  ina219_2.setCorrectionFactor(ina219_2_correctionFactor);

  ina219_3.reset_INA219();
  ina219_3.init();
  ina219_3.setBusRange(BRNG_32);
  ina219_3.setCorrectionFactor(ina219_3_correctionFactor);

  ina219_4.reset_INA219();
  ina219_4.init();
  ina219_4.setBusRange(BRNG_32);
  ina219_4.setCorrectionFactor(ina219_4_correctionFactor);

  delay(1000);

}
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{

   // Stop further decoding as image is running off bottom of screen
  if ( y >= tft.height() ) return 0;

  // This function will clip the image block rendering automatically at the TFT boundaries
  tft.pushImage(x, y, w, h, bitmap);

  // This might work instead if you adapt the sketch to use the Adafruit_GFX library
  // tft.drawRGBBitmap(x, y, bitmap, w, h);

  // Return 1 to decode next block
  return 1;
}
void setupWeb()
{
  Serial.print("Starting Web Server Hub...\n\n");
  webServer.begin();

  //on reset unpair homekit devices
  webServer.on("/reset", []()
  {
      String content = "<html><body><h1><br>Device unpaired! Please return to HomeKit and try to repair device</h1></body></html>";
      webServer.send(200, "text/html", content);
      delay(5000);

      HAPClient::removeControllers();                                                                           // clear all Controller data
      nvs_set_blob(HAPClient::hapNVS,"CONTROLLERS",HAPClient::controllers,sizeof(HAPClient::controllers));      // update data
      nvs_commit(HAPClient::hapNVS);                                                                            // commit to NVS
      Serial.println("############# HomeSpan Pairing Data DELETED ***\n\n");

      ESP.restart();
  });
}// setupWeb
