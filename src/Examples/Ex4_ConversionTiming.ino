/******************************************************************************
  Ex4_ConversionTiming.ino
  Example for the TMP117 I2C Temperature Sensor
  Trevor Sprague
  Sept 10 2021

  This example sets a variety of conversion times and averaging samples.

  Resources:
  Wire.h (included with Arduino IDE)
  tmp117.h (included in the src folder)
******************************************************************************/

#include <Arduino.h>
#include "tmp117.h"
#include <Wire.h>

#define TMP_ADDR 0x48

TMP117 ts = TMP117(TMP_ADDR);

// This returns the number of samples averaged
// rather than the bits in the convfig register
int numSamplesAveraged(uint8_t avg);

void setup() {
  Serial.begin(9600);
  Wire.begin();

  if(!ts.setup()){
    //if the device is not found or an issue with i2c bus, pause function indefinitely
    Serial.println("Error connecting with device! Pausing function.");
    while(1);
  }
  ts.softReset(); //reset sets high temp limit = 192*C, low limit = -256*C
}

void loop() {
  // Iterate through all conversion cycle time and sample averaging combinations.
  // The conversion cycle time is how long between readings.
  // Averaging refers to how many readings to consecutively take, then average.
  // See end of example for list of cc times and number of averaging samples

  float convTime;

  for(int i = 0; i < 4; i++){
    ts.setAveraging(i);
    Serial.print(numSamplesAveraged(ts.getAveraging()));
    Serial.println(" samples will be averaged.");

    for(int j = 0; j < 8; j++){
      ts.setConversionCycleTime(j);
      convTime = ts.getConversionTime();

      delay(convTime);  // wait until the reading is complete
      Serial.print(convTime, 1);
      Serial.print(" ms conversion time reading: ");
      Serial.print(ts.readTempC());
      Serial.println(" C");
    }
  }
}

int numSamplesAveraged(uint8_t avg){
  switch(avg){
    case(0):
      return 1;
    case(1):
      return 8;
    case(2):
      return 32;
    case(3):
      return 64;
    default:
      return 8;
  }
}

// Accepted arguments for setting number of samples to average (default 1)
//   avgNone = 0 === take a single reading
//   avg8 = 1 === average 8 readings
//   avg32 = 2 === average 32 readings
//   avg64 = 3 === average 64 readings

// Accepted arguments for setting conversion cycle time (default 1s)
//   conv15p5ms = 0b000 === conversion cycle takes 15.5ms
//   conv125ms = 0b001 === ...takes 125ms
//   conv250ms = 0b010 === ...takes 250ms
//   conv500ms = 0b011 === ...takes 500ms
//   conv1s = 0b100 === ...takes 1000ms
//   conv4s = 0b101 === ...takes 4000ms
//   conv8s = 0b110 === ...takes 8000ms
//   conv16s = 0b111 === ...takes 16000ms