/******************************************************************************
  Ex3_Alerts.ino
  Example for the TMP117 I2C Temperature Sensor
  Trevor Sprague
  Sept 10 2021

  This example sets an upper and lower temperature limit and checks if the
  temperature is outside the limits.

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
    //if the device is not found or an issue with i2c bus, pause function indefinitely
    Serial.println("Error connecting with device! Pausing function.");
    while(1);
  }
  ts.softReset(); //reset sets high temp limit = 192*C, low limit = -256*C
}

void loop() {
  //read the alert flag BEFORE it's cleared by other commands:
  //when in alert mode (therm mode off)
  //if the high/low limit is crossed (alert = 1), but the config reg
  //is read in a different command prior to isHighAlert()/isLowAlert(),
  //then the alert will be read as 0

  // get conversion time
  float convTime = ts.getConversionTime();
  Serial.print("Conversion time: ");
  Serial.print(convTime, 1);
  Serial.println(" ms");

  // set high/low limits to +/-10C from current temperature
  ts.setTempOffset(0);
  delay(convTime);
  float initTemp = ts.readTempC();
  ts.setTempHighLimit(initTemp+10);
  ts.setTempLowLimit(initTemp-10);

  // display temperature limits
  Serial.println();
  Serial.print("High Temp Limit: ");
  Serial.print(ts.getTempHighLimit(), 2);
  Serial.println(" C");
  Serial.print("Low Temp Limit: ");
  Serial.print(ts.getTempLowLimit(), 2);
  Serial.println(" C");
  delay(1000);

  // decrement offset past the lower limit to force the low alert
  // (this allows limits to be tested without physically changing the temp)
  for(int i = 0; i >= -20; i-=2){
    ts.setTempOffset(i);
    delay(convTime);  // wait until the reading is complete

    Serial.print(ts.getTempOffset());
    Serial.print(" C offset temp: ");
    Serial.print(ts.readTempC());
    Serial.println(" C");

    Serial.print("Low alert: ");
    Serial.println(ts.isLowAlert());
    // Both high and low alerts can be checked simultaneously with:
    // ts.getHighLowAlert()
    // Running any of isLowAlert(), isHighAlert(), getHighLowAlert()
    // clears the config register, so only one can be used per reading
  }

  // increment offset past the upper limit to force the high alert
  // (this allows limits to be tested without physically changing the temp)
  for(int i = 0; i <= 20; i+=2){
    ts.setTempOffset(i);
    delay(convTime);  // wait until the reading is complete

    Serial.print(ts.getTempOffset());
    Serial.print(" C offset temp: ");
    Serial.print(ts.readTempC());
    Serial.println(" C");

    Serial.print("High alert: ");
    Serial.println(ts.isHighAlert());
  }
}