/******************************************************************************
  Ex1_BasicReading.ino
  Example for the TMP117 I2C Temperature Sensor
  Trevor Sprague
  Sept 5 2021

  This example captures and prints the
  temperature in degrees celsius and fahrenheit.

  Resources:
  Wire.h (included with Arduino IDE)
  tmp117.h (included in the src folder)
******************************************************************************/

#include <Arduino.h>
#include "tmp117.h"
#include <Wire.h>

#define TMP_ADDR 0x48

TMP117 ts = TMP117(TMP_ADDR);

void printTemp(String tempStr, float temp);

void setup() {
  Serial.begin(9600);
  Wire.begin();

  if(!ts.setup()){
    // if the device is not found or an issue with i2c bus, pause function indefinitely
    Serial.println("Error connecting with device! Pausing function.");
    while(1);
  }
}

void loop() {
  float tempC, tempF;

  if(ts.isDataReady()){       // wait until a reading is taken
    tempC = ts.readTempC();
    tempF = ts.readTempF();
    printTemp("C", tempC);
    printTemp("F", tempF);
    Serial.println();
  }
}

void printTemp(String tempStr, float temp){
  Serial.print("Temp " + tempStr + " = ");
  Serial.println(temp);
}