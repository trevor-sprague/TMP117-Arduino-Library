#ifndef _TMP117_H_
#define _TMP117_H_

#include <Arduino.h>
#include <Wire.h>

#define REG_TEMP        0x00
#define REG_CONFIG      0X01
#define REG_T_HIGH_LIM  0x02
#define REG_T_LOW_LIM   0X03

#define REG_EEPROM_UL   0x04
#define REG_EEPROM1     0X05
#define REG_EEPROM2     0x06
#define REG_TEMP_OFFSET 0X07
#define REG_EEPROM3     0x08
#define REG_DEV_ID      0X0F

#define TEMP_RESOLUTION 0.0078125f
#define CONVERT_C_TO_F  1.8f
#define DEVICE_ID       0x0117

enum avgConversions{
    // default 0b01
    avgNone = 0b00,
    avg8 = 0b01,
    avg32 = 0b10,
    avg64 = 0b11
};

enum conversionCylceTime{
    // default 0b100
    conv15p5ms = 0b000,
    conv125ms = 0b001,
    conv250ms = 0b010,
    conv500ms = 0b011,
    conv1s = 0b100,
    conv4s = 0b101,
    conv8s = 0b110,
    conv16s = 0b111
};

enum conversionMode{
    // default 0b00
    ccMode = 0b00,
    sdMode = 0b01,
    ccMode2 = 0b10,
    osMode = 0b11
};

class TMP117{
    public:
        TMP117(uint8_t i2cAddr=0x48);
        bool setup();
        float readTempC();
        float readTempF();

        uint16_t getDeviceID();
        void setConfig(uint16_t data);
        uint16_t getConfig();
        void softReset();
        bool isDataReady();
        bool isEEPROMBusy();

        void setTempHighLimit(float hTempC);
        float getTempHighLimit();
        void setTempLowLimit(float lTempC);
        float getTempLowLimit();
        void setTempOffset(float offTempC);
        float getTempOffset();
        
        uint8_t getHighLowAlert();
        bool isHighAlert();
        bool isLowAlert();
        void setThermAlert(bool alertOn=0);
        bool getThermAlert();
        void setAlertPolarity(bool polarity=0);
        bool getAlertPolarity();
        void setAlertPin(bool alertPin=0);
        bool getAlertPin();

        float getConversionTime();
        void setAveraging(avgConversions avgSelect=avg8);
        void setAveraging(uint8_t avgSelect=1);
        uint8_t getAveraging();
        void setConversionCycleTime(conversionCylceTime ccTime=conv1s);
        void setConversionCycleTime(uint8_t ccTime=4);
        uint8_t getConversionCycleTime();
        void setConversionMode(conversionMode mode=ccMode);
        void setConversionMode(uint8_t mode=0);
        uint8_t getConversionMode();

    private:
        uint8_t _i2cAddr;

        uint16_t readReg(uint8_t reg);
        void writeReg(uint8_t reg, uint16_t data);

        float calculateTempC(int16_t rawData);
        float calculateTempF(int16_t rawData);
};

#endif