
////////////////////////////////////
//   DEVICE-SPECIFIC SERVICES //
////////////////////////////////////
SpanCharacteristic *Discharger_temperature;
/////
SpanCharacteristic *Lipo1_Voltage;
SpanCharacteristic *Lipo1_BatteryLevel;
SpanCharacteristic *Lipo1_StatusLowBattery;
SpanCharacteristic *Lipo1_hum_Percentage;
SpanCharacteristic *Lipo1_hum_StatusLowBattery;
/////
SpanCharacteristic *Lipo2_Voltage;
SpanCharacteristic *Lipo2_BatteryLevel;
SpanCharacteristic *Lipo2_StatusLowBattery;
SpanCharacteristic *Lipo2_hum_Percentage;
SpanCharacteristic *Lipo2_hum_StatusLowBattery;
/////
SpanCharacteristic *Lipo3_Voltage;
SpanCharacteristic *Lipo3_BatteryLevel;
SpanCharacteristic *Lipo3_StatusLowBattery;
SpanCharacteristic *Lipo3_hum_Percentage;
SpanCharacteristic *Lipo3_hum_StatusLowBattery;
/////
SpanCharacteristic *Lipo4_Voltage;
SpanCharacteristic *Lipo4_BatteryLevel;
SpanCharacteristic *Lipo4_StatusLowBattery;
SpanCharacteristic *Lipo4_hum_Percentage;
SpanCharacteristic *Lipo4_hum_StatusLowBattery;



struct DEV_Discharger_temp : Service::TemperatureSensor {

  DEV_Discharger_temp() : Service::TemperatureSensor()
  {
    Discharger_temperature=new Characteristic::CurrentTemperature(0);
  }
};
////////////////////////////////////////////////
struct DEV_Lipo1_temp : Service::TemperatureSensor {

  DEV_Lipo1_temp() : Service::TemperatureSensor()
  {
    Lipo1_Voltage=new Characteristic::CurrentTemperature(0);
    Lipo1_BatteryLevel=new Characteristic::BatteryLevel(0);
    Lipo1_StatusLowBattery=new Characteristic::StatusLowBattery(0);
  }
};
struct DEV_Lipo1_hum : Service::HumiditySensor {

  DEV_Lipo1_hum() : Service::HumiditySensor()
  {
    Lipo1_hum_Percentage=new Characteristic::CurrentRelativeHumidity(0);
    Lipo1_hum_StatusLowBattery=new Characteristic::StatusLowBattery(0);
  }
};
////////////////////////////////////////////////
struct DEV_Lipo2_temp : Service::TemperatureSensor {

  DEV_Lipo2_temp() : Service::TemperatureSensor()
  {
    Lipo2_Voltage=new Characteristic::CurrentTemperature(0);
    Lipo2_BatteryLevel=new Characteristic::BatteryLevel(0);
    Lipo2_StatusLowBattery=new Characteristic::StatusLowBattery(0);
  }
};
struct DEV_Lipo2_hum : Service::HumiditySensor {

  DEV_Lipo2_hum() : Service::HumiditySensor()
  {
    Lipo2_hum_Percentage=new Characteristic::CurrentRelativeHumidity(0);
    Lipo2_hum_StatusLowBattery=new Characteristic::StatusLowBattery(0);
  }
};
////////////////////////////////////////////////

struct DEV_Lipo3_temp : Service::TemperatureSensor {

  DEV_Lipo3_temp() : Service::TemperatureSensor()
  {
    Lipo3_Voltage=new Characteristic::CurrentTemperature(0);
    Lipo3_BatteryLevel=new Characteristic::BatteryLevel(0);
    Lipo3_StatusLowBattery=new Characteristic::StatusLowBattery(0);
  }
};
struct DEV_Lipo3_hum : Service::HumiditySensor {

  DEV_Lipo3_hum() : Service::HumiditySensor()
  {
    Lipo3_hum_Percentage=new Characteristic::CurrentRelativeHumidity(0);
    Lipo3_hum_StatusLowBattery=new Characteristic::StatusLowBattery(0);
  }
};
////////////////////////////////////////////////

struct DEV_Lipo4_temp : Service::TemperatureSensor {

  DEV_Lipo4_temp() : Service::TemperatureSensor()
  {
    Lipo4_Voltage=new Characteristic::CurrentTemperature();
    Lipo4_BatteryLevel=new Characteristic::BatteryLevel(0);
    Lipo4_StatusLowBattery=new Characteristic::StatusLowBattery(0);
  }
};
struct DEV_Lipo4_hum : Service::HumiditySensor {

  DEV_Lipo4_hum() : Service::HumiditySensor()
  {
    Lipo4_hum_Percentage=new Characteristic::CurrentRelativeHumidity(0);
    Lipo4_hum_StatusLowBattery=new Characteristic::StatusLowBattery(0);
  }
};
