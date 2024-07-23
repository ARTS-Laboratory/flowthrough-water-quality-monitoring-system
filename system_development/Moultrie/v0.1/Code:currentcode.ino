#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <SD.h>
#include <EEPROM.h>
#include <GravityTDS.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DS323x.h>
#include <DFRobot_PH.h>

#define TdsSensorPin A1
#define SEALEVELPRESSURE_HPA (1013.25)

uint8_t user_bytes_received = 0;
const uint8_t bufferlen = 32;
char user_data[bufferlen];

void parse_cmd(char* string) {
  strupr(string);
}

DS323x rtc;
GravityTDS gravityTds;
DFRobot_PH phSensor;
const int chipSelect = 27;
float temperature = 25, tdsValue = 0;
File myFile;

const int SENSOR_PIN = 7; // Pin for temperature sensor

OneWire oneWire(SENSOR_PIN);
DallasTemperature tempSensor(&oneWire);

float tempCelsius;

void setup() {
  Serial.begin(115200);
  pinMode(9, OUTPUT);
  pinMode(3, OUTPUT);
  
  tempSensor.begin();
  gravityTds.setPin(TdsSensorPin);
  gravityTds.setAref(5.0); // reference voltage on ADC, default 5.0V on Arduino UNO
  gravityTds.setAdcRange(1024); // 1024 for 10bit ADC; 4096 for 12bit ADC
  gravityTds.begin(); // initialization
  
  phSensor.begin();

  Wire.begin();
  rtc.attach(Wire);
  rtc.now(DateTime(2022, 10, 7, 12, 45, 0));

  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed!");
    while (true);
  }
}

void loop() {
  digitalWrite(9, HIGH);
  digitalWrite(3, HIGH);

  delay(60000); // 1 minute delay

  digitalWrite(9, LOW);
  digitalWrite(3, LOW);

  delay(1000); // 1 second delay
    
  tempSensor.requestTemperatures(); // send the command to get temperatures
  tempCelsius = tempSensor.getTempCByIndex(0); // read temperature in Celsius
  
  gravityTds.setTemperature(tempCelsius); // set the temperature and execute temperature compensation
  gravityTds.update(); // sample and calculate
  tdsValue = gravityTds.getTdsValue(); // get the value

  // Read pH value
  float voltage = analogRead(A0) * (5.0 / 1024.0); // Convert analog reading to voltage
  float pHValue = phSensor.readPH(voltage, tempCelsius);

  if (Serial.available() > 0) {
    user_bytes_received = Serial.readBytesUntil(13, user_data, sizeof(user_data));
  }

  if (user_bytes_received) {
    parse_cmd(user_data);
    user_bytes_received = 0;
    memset(user_data, 0, sizeof(user_data));
  }

  DateTime now = rtc.now();
  Serial.println(now.timestamp());

  File dataFile = SD.open("data.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.print(tdsValue, 0);
    dataFile.print(",");
    dataFile.print(" ");
    dataFile.print("ppm");
    dataFile.print(",");
    dataFile.print(" ");
    dataFile.print(pHValue);
    dataFile.print(",");
    dataFile.print(" ");
    dataFile.print(tempCelsius);
    dataFile.print(",");
    dataFile.print(" ");
    dataFile.print("°C");
    dataFile.print(",");
    dataFile.print(" ");
    dataFile.println(now.timestamp());
    
    dataFile.close();
  } else {
    Serial.println("Error opening data.csv");
  }

  delay(240000); // 4 minute delay
}
