#include <Arduino.h>
#include <SPI.h>
#include "NRF24.hpp"

namespace NRF24
{

    // Platform specific implementations
    class Arduino : public Driver_Base
    {
    public:
        Arduino(int ChipSelectPin, int ChipEnablePin)
        {
            CSN_PIN = ChipSelectPin;
            CE_PIN = ChipEnablePin;

      	    // Set CE and CSN pins as outputs 
      	    pinMode(CE_PIN, OUTPUT);
      	    pinMode(CSN_PIN, OUTPUT);
        }

    public:
    	void SPI_BeginTransaction() override
    	{
    		SPI.beginTransaction(SPISettings(10e6, MSBFIRST, SPI_MODE0));
    	}

    	void SPI_EndTransaction() override
    	{
    		SPI.endTransaction();
    	}

    	uint8_t SPI_Transfer(uint8_t data) override
    	{
    		return SPI.transfer(data);
    	}

    	void setCSN(bool state) override
        {
            digitalWrite(CSN_PIN, state);
        }

    	void setCE(bool state) override
        {
            digitalWrite(CE_PIN, state);
        }

        void delayMicro(unsigned int microSeconds) override
        {
            delayMicroseconds(microSeconds);
        }

        void print(const char* chars) override
        {
            Serial.print(chars);
        }

        void print(int val) override
        {
            Serial.print(val);
        }

        void printHex(int val) override
        {
            Serial.print(val, HEX);
        }
    };
}