#pragma once
#include <stdint.h>

class NRF24_Driver_Base
{
public:
	NRF24_Driver_Base();
	~NRF24_Driver_Base();

	void init();

	void powerOn();
	void setPayloadWidth();
	void SetModeReceive();
	void writePayload(uint8_t* data, int size);
	void readPayload(uint8_t* data, int size);
	bool txFull();
	bool txEmpty();
	bool rxDataReady();
	bool txTransmit();

	void flushTx();
	void flushRx();

	void softReset();
	void printPrettyConfig();
	void printPrettyStatus();
	void printPrettyRxAdresses();

protected:
	int CSN_PIN;
	int CE_PIN;

	virtual void SPI_BeginTransaction() = 0;
	virtual void SPI_EndTransaction() = 0;
	virtual uint8_t SPI_Transfer(uint8_t data) = 0;

	virtual void setCSN(bool state) = 0;
	virtual void setCE(bool state) = 0;

	virtual void delayMicro(unsigned int microSeconds) = 0;
	virtual void print(const char* chars) = 0;
	virtual void print(int val) = 0;
	virtual void printHex(int val) = 0; 
	
public: // Will be private
	uint8_t m_ReadRegister(uint8_t reg);
	void m_ReadMultiByteRegister(uint8_t reg, uint8_t* bytes, int size);

	void m_WriteRegister(uint8_t reg, uint8_t value);
	void m_WriteMultiByteRegister(uint8_t reg, uint8_t* bytes, int size);
};



