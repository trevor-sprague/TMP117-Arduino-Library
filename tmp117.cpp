#include <Arduino.h>
#include <Wire.h>
#include <tmp117.h>

TMP117::TMP117(uint8_t i2cAddr){
    _i2cAddr = i2cAddr;
}

bool TMP117::setup(){
    Wire.beginTransmission(_i2cAddr);
    if(Wire.endTransmission())
        return false;

    if(getDeviceID() != DEVICE_ID)
        return false;

    return true;
}

uint16_t TMP117::readReg(uint8_t reg){
    Wire.beginTransmission(_i2cAddr);
    Wire.write(reg);
    Wire.endTransmission();

    Wire.requestFrom(_i2cAddr, (uint8_t)2);
    int16_t regData = ((Wire.read() << 8) | Wire.read());

    return regData;
}

void TMP117::writeReg(uint8_t reg, uint16_t data){
    Wire.beginTransmission(_i2cAddr);
    Wire.write(reg);
    Wire.write(highByte(data));
    Wire.write(lowByte(data));
    Wire.endTransmission();
    return;
}

float TMP117::calculateTempC(int16_t rawData){
    float tempC = rawData * TEMP_RESOLUTION;
    return tempC;
}

float TMP117::calculateTempF(int16_t rawData){
    float tempC = rawData * TEMP_RESOLUTION;
    float tempF = tempC * CONVERT_C_TO_F + 32.0;
    return tempF;
}

float TMP117::readTempC(){
    int16_t rawData = readReg(REG_TEMP);
    float tempC = calculateTempC(rawData);
    return tempC;
}

float TMP117::readTempF(){
    int16_t rawData = readReg(REG_TEMP);
    float tempF = calculateTempF(rawData);
    return tempF;
}


uint16_t TMP117::getDeviceID(){
    return readReg(REG_DEV_ID);
}

void TMP117::setConfig(uint16_t data){
    writeReg(REG_CONFIG, data);
    return;
}

uint16_t TMP117::getConfig(){
    return readReg(REG_CONFIG);
}

void TMP117::softReset(){
    int16_t configData = getConfig();
    writeReg(REG_CONFIG, bitWrite(configData, 1, 1));
    while(isEEPROMBusy());  //wait until reset is complete
    return;
}

bool TMP117::isDataReady(){
    return bitRead(getConfig(), 13);
}

bool TMP117::isEEPROMBusy(){
    return bitRead(getConfig(), 12);
}


void TMP117::setTempHighLimit(float hTempC){
    float tempCount = (float)hTempC / TEMP_RESOLUTION;
    int16_t regCount = round(tempCount);
    writeReg(REG_T_HIGH_LIM, regCount);
    return;
}

float TMP117::getTempHighLimit(){
    return ((int16_t)readReg(REG_T_HIGH_LIM) * TEMP_RESOLUTION);
}

void TMP117::setTempLowLimit(float lTempC){
    float tempCount = (float)lTempC / TEMP_RESOLUTION;
    int16_t regCount = round(tempCount);
    writeReg(REG_T_LOW_LIM, regCount);
    return;
}

float TMP117::getTempLowLimit(){
    return ((int16_t)readReg(REG_T_LOW_LIM) * TEMP_RESOLUTION);
}

void TMP117::setTempOffset(float offTempC){
    float tempCount = (float)offTempC / TEMP_RESOLUTION;
    int16_t regCount = round(tempCount);
    writeReg(REG_TEMP_OFFSET, regCount);
    return;
}

float TMP117::getTempOffset(){
    return ((int16_t)readReg(REG_TEMP_OFFSET) * TEMP_RESOLUTION);
}


uint8_t TMP117::getHighLowAlert(){
    uint16_t cfg = getConfig();

    uint8_t highLowAlert = 0;
    bitWrite(highLowAlert, 1, bitRead(cfg, 15));
    bitWrite(highLowAlert, 0, bitRead(cfg, 14));

    return highLowAlert;
}

bool TMP117::isHighAlert(){
    return bitRead(getConfig(), 15);
}

bool TMP117::isLowAlert(){
    return bitRead(getConfig(), 14);
}

void TMP117::setThermAlert(bool alertOn){
    int16_t configData = getConfig();
    writeReg(REG_CONFIG, bitWrite(configData, 4, alertOn));
    return;
}

bool TMP117::getThermAlert(){
    return bitRead(getConfig(), 4);
}

void TMP117::setAlertPolarity(bool polarity){
    int16_t configData = getConfig();
    writeReg(REG_CONFIG, bitWrite(configData, 3, polarity));
    return;
}

bool TMP117::getAlertPolarity(){
    return bitRead(getConfig(), 3);
}

void TMP117::setAlertPin(bool alertPin){
    int16_t configData = getConfig();
    writeReg(REG_CONFIG, bitWrite(configData, 2, alertPin));
    return;
}

bool TMP117::getAlertPin(){
    return bitRead(getConfig(), 2);
}


float TMP117::getConversionTime(){
    uint8_t ccTime = getConversionCycleTime();
    uint8_t avgConv = getAveraging();
    // conversion cycle time in ms
    float convTable[8][4] = {{15.5, 125, 500, 1000},
                             {125, 125, 500, 1000},
                             {250, 250, 500, 1000},
                             {500, 500, 500, 1000},
                             {1000, 1000, 1000, 1000},
                             {4000, 4000, 4000, 4000},
                             {8000, 8000, 8000, 8000},
                             {16000, 16000, 16000, 16000}};
    
    // int rows = sizeof convTable / sizeof convTable[0];  
    // int cols = sizeof convTable[0] / sizeof(int);

    return(convTable[ccTime][avgConv]);
}

void TMP117::setAveraging(avgConversions avgSelect){
    uint16_t configData = getConfig();
    uint16_t newCfg = bitWrite(configData, 5, bitRead(avgSelect, 0));
    newCfg = bitWrite(newCfg, 6, bitRead(avgSelect, 1));
    writeReg(REG_CONFIG, newCfg);
    return;
}

void TMP117::setAveraging(uint8_t avgSelect){
    uint16_t configData = getConfig();
    uint16_t newCfg = bitWrite(configData, 5, bitRead(avgSelect, 0));
    newCfg = bitWrite(newCfg, 6, bitRead(avgSelect, 1));
    writeReg(REG_CONFIG, newCfg);
    return;
}

uint8_t TMP117::getAveraging(){
    uint16_t configData = getConfig();
    uint8_t avgConv = (bitRead(configData, 6) << 1) | bitRead(configData, 5);
    return(avgConv);
}

void TMP117::setConversionCycleTime(conversionCylceTime ccTime){
    uint16_t configData = getConfig();
    uint16_t newCfg = bitWrite(configData, 7, bitRead(ccTime, 0));
    newCfg = bitWrite(newCfg, 8, bitRead(ccTime, 1));
    newCfg = bitWrite(newCfg, 9, bitRead(ccTime, 2));
    writeReg(REG_CONFIG, newCfg);
    return;
}

void TMP117::setConversionCycleTime(uint8_t ccTime){
    uint16_t configData = getConfig();
    uint16_t newCfg = bitWrite(configData, 7, bitRead(ccTime, 0));
    newCfg = bitWrite(newCfg, 8, bitRead(ccTime, 1));
    newCfg = bitWrite(newCfg, 9, bitRead(ccTime, 2));
    writeReg(REG_CONFIG, newCfg);
    return;
}

uint8_t TMP117::getConversionCycleTime(){
    uint16_t configData = getConfig();
    uint8_t avgCCTime = (((bitRead(configData, 9) << 1) |
                        bitRead(configData, 8)) << 1) |
                        bitRead(configData, 7);
    return(avgCCTime);
}

void TMP117::setConversionMode(conversionMode mode){
    uint16_t configData = getConfig();
    uint16_t newCfg = bitWrite(configData, 10, bitRead(mode, 0));
    newCfg = bitWrite(newCfg, 11, bitRead(mode, 1));
    writeReg(REG_CONFIG, newCfg);
    return;
}

void TMP117::setConversionMode(uint8_t mode){
    uint16_t configData = getConfig();
    uint16_t newCfg = bitWrite(configData, 10, bitRead(mode, 0));
    newCfg = bitWrite(newCfg, 11, bitRead(mode, 1));
    writeReg(REG_CONFIG, newCfg);
    return;
}

uint8_t TMP117::getConversionMode(){
    uint16_t configData = getConfig();
    uint8_t convMode = (bitRead(configData, 11) << 1) | bitRead(configData, 10);
    return(convMode);
}
