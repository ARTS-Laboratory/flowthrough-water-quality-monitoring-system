//Water Height Sensor developed by Nicholas Liger, from previous work by Corrine Smith and Winford Janvrin
//in conjuction with ARTS - LAB at University of South and Carolina for the flowthrough NMR project

#include <SPI.h>  //SPI communication library
#include <SD.h> //SD card library
#include <Wire.h> //Wire library for communicating with I2C and the RTC
#include <DS3231.h> //library for RTC module
#include <LowPower.h>
#include "NewPing.h" //New Ping library for use with the sonar sensors

const int trigPin1 = 0; //trigger pin for sonar sensor 1
const int echoPin1 = 0; //echo pin for sonar sensor 1
const int maxDist = 450; // max distance for sonar sensor, same for both
NewPing sonar_sensor_1(trigPin1, echoPin1, maxDist); //set up NewPing variable for sonar sensor 1
float distance1=0; //variable for distance measured by sonar sensor 1 used to publish to Adafruit IO via MQTT 
float origDist1=0; //Original distance for sensor 1 that is used to calibrate the height measurement of the water
float origHeight1 = 0; //Height of the water beneath sensor 1, measured at the time of installation
float height1 = 0; //Height variable for the current water height calculated from distance1

//Variables for setting up the SD card and RTC
char timeStamp[32]; //Char/string variable for holding time stamp
const int chipSelect = 53; //Chip Select for the SD card
char dayStampFileName[20]; //Char/string variable for holding the day stamp for naming
//file in SD card
RTClib myRTC; //RTC variable for using DS3231 library scripts
uint8_t currentTime = 1;
uint8_t lastTime = 1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); //begin the Serial monitor
  Wire.begin(); //begin communication with I2C, needed for RTC module
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
  origDist1 = sonar_sensor_1.ping_cm();
}

void loop() {
  // put your main code here, to run repeatedly:
  DateTime now = myRTC.now();  
  currentTime = now.minute();
  if (currentTime %5 == 0 && currentTime != lastTime ){
    //This section creates the timestamps from the RTC module
    sprintf(timeStamp, "%02d:%02d:%02d %02d/%02d/%02d", now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year());  
    snprintf(dayStampFileName, 20, "%02d%02d%02d.txt", now.day(), now.month(), now.year());  
    Serial.print(F("Date/Time: "));
    Serial.println(timeStamp);
    distance1 = sonar_sensor_1.ping_cm();
    height1 = origHeight1 + (origDist1 - distance1)/(2.54*12);
    // This section saves the data to the SD card
    Serial.println(dayStampFileName);
    //This line creates a new file named with the day time stamp (DDMMYYYY)
    //or opens the file if this name already exists
    //The SD cards have a tendency to become corrupted which can ruin a long term test
    //if the data is all saved to a single file, so this method creates a new CSV for every day
    //so that even if a file is corrupted, we will still have the data from every other day
    if (SD.exists(dayStampFileName) == 0) {
      File dataFile = SD.open(dayStampFileName, FILE_WRITE);
      if (dataFile){
        dataFile.println("Ultrasonic 1 Height Reading (ft), Timestamp");
        dataFile.close();
      }
      else {
      Serial.println("error opening datalog.txt");
      }
    }
    File dataFile = SD.open(dayStampFileName, FILE_WRITE);
    if (dataFile) {
        dataFile.print(height1);
        dataFile.print(",");
        dataFile.println(timeStamp);
        dataFile.close();
      }
    // if the file isn't open, pop up an error:
    else {
      Serial.println("error opening datalog.txt");
    }
  }
  else {
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  }
}
