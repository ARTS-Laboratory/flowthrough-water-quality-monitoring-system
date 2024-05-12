#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <SD.h>
#include <EEPROM.h>
#include <GravityTDS.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define TdsSensorPin A1
#include <DS323x.h>
#define SEALEVELPRESSURE_HPA (1013.25)

#ifdef USE_PULSE_OUT
  #include "ph_iso_grav.h"       
  Gravity_pH_Isolated pH = Gravity_pH_Isolated(A0);         
#else
  #include "ph_grav.h"             
  Gravity_pH pH = Gravity_pH(A0);   
#endif
                

uint8_t user_bytes_received = 0;                
const uint8_t bufferlen = 32;                   
char user_data[bufferlen];                     

void parse_cmd(char* string) {                   
  strupr(string);                                
}                   

Adafruit_BME280 bme;
DS323x rtc;
GravityTDS gravityTds;

const int chipSelect = 10;
float temperature = 25, tdsValue = 0;
File myFile;
void save_temperature() {}

const int SENSOR_PIN = 7; 

OneWire oneWire(SENSOR_PIN);         
DallasTemperature tempSensor(&oneWire); 

float tempCelsius;    
float tempFahrenheit; 


void setup() 
{
  
  Serial.begin(115200);
  pinMode(9,OUTPUT);
  pinMode(3,OUTPUT);
  
  tempSensor.begin();
  gravityTds.setPin(TdsSensorPin);
  gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
  gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
  gravityTds.begin();  //initialization
   
  
  Wire.begin();
  if (pH.begin()) {                                     
    Serial.println("Loaded EEPROM");
  }
   rtc.attach(Wire);
   rtc.now(DateTime(2022, 2, 2, 6, 42,0));
  Wire.begin();
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
   while (!Serial);

  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    while (true);
  }

  Serial.println("initialization done.");
  
 
}

void loop() {
 
  digitalWrite(9,HIGH);
  digitalWrite(3,HIGH);
  
  
  tempSensor.requestTemperatures();             // send the command to get temperatures
  tempCelsius = tempSensor.getTempCByIndex(0);  // read temperature in Celsius
  tempFahrenheit = tempCelsius * 9 / 5 + 32; // convert Celsius to Fahrenheit
  
  gravityTds.setTemperature(temperature);  // set the temperature and execute temperature compensation
  gravityTds.update();  //sample and calculate
  tdsValue = gravityTds.getTdsValue();  // then get the value

  if (Serial.available() > 0) {                                                      
    user_bytes_received = Serial.readBytesUntil(13, user_data, sizeof(user_data));   
  }

  if (user_bytes_received) {                                                      
    parse_cmd(user_data);                                                          
    user_bytes_received = 0;                                                        
    memset(user_data, 0, sizeof(user_data));                                         
  }

  
  
  Serial.print(tdsValue, 0);
  Serial.print("ppm");
  Serial.print(",");
  Serial.print(" ");
  Serial.print(pH.read_ph());
  Serial.print("ph ");
  Serial.print(",");
  Serial.print(" ");
  Serial.print(" Ambient Temp = ");
  Serial.print(bme.readTemperature());
  Serial.println("*C");
  Serial.print(",");
  Serial.print(" ");
  Serial.print("Temperature: ");
  Serial.print(tempCelsius);    // print the temperature in Celsius
  Serial.print("°C");
  Serial.print(",");
  Serial.print(" ");        // separator between Celsius and Fahrenheit
  DateTime now = rtc.now();
  Serial.println(now.timestamp());
  
  File dataFile = SD.open("sheriff.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.print(tdsValue, 0);
    dataFile.print("ppm");
    dataFile.print(",");
    dataFile.print(" ");
    dataFile.print(pH.read_ph());
    dataFile.print(",");
    dataFile.print(" ");
    dataFile.print(tempCelsius);
    dataFile.print("°C");
    dataFile.print(",");
    dataFile.print(" ");
    dataFile.print(" Ambient Temp = ");
    dataFile.print(bme.readTemperature());
    dataFile.print("*C");
    dataFile.print(",");
    dataFile.print(" ");
    dataFile.println(now.timestamp());
    
    dataFile.close();
  }
  else {
    Serial.println("error opening datalog.txt");
  }

 delay(5000);
  
}
