/******************************************************************************
  Ex2_TempOffset.ino
  Example for the TMP117 I2C Temperature Sensor
  Trevor Sprague
  Sept 5 2021

  This example sets a variety of temperature offsets and captures the
  temperature at each offset (in degrees celcius).

  Resources:
  Wire.h (included with Arduino IDE)
  tmp117.h (included in the src folder)
******************************************************************************/

#include <Arduino.h>
#include "tmp117.h"
#include <Wire.h>

#define TMP_ADDR 0x48

TMP117 ts = TMP117(TMP_ADDR);

void setup() {
  Serial.begin(9600);
  Wire.begin();

  if(!ts.setup()){
    // if the device is not found or an issue with i2c bus, pause function indefinitely
    Serial.println("Error connecting with device! Pausing function.");
    while(1);
  }
  ts.softReset(); // software reset sets high temp limit = 192*C, low limit = -256*C
}

void loop() {
  // reset temperature offset to 0C
  ts.setTempOffset(0);
  ts.readTempC();           // clear data registers
  while(!ts.isDataReady()); // wait until data is captured/ready
  Serial.print("Initial temp: ");
  Serial.print(ts.readTempC());
  Serial.println(" C");

  // increase temperature offset up to 10C in 1C increments
  // and take measurements at each offset
  for(int i = 0; i <= 10; i++){
    ts.setTempOffset(i);

    while(!ts.isDataReady());
    Serial.print(ts.getTempOffset());
    Serial.print(" C offset temp: ");
    Serial.print(ts.readTempC());
    Serial.println(" C");
  }

  // decrease temperature offset down to -10C in 1C increments
  // and take measurements at each offset
  for(int i = 0; i >= -10; i--){
    ts.setTempOffset(i);

    while(!ts.isDataReady());
    Serial.print(ts.getTempOffset());
    Serial.print(" C offset temp: ");
    Serial.print(ts.readTempC());
    Serial.println(" C");
  }
  
  Serial.println();
  Serial.println();
  delay(5000);
}