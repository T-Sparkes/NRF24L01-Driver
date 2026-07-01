#pragma once
#include <stdint.h>

/*
TODO: 
- Pipe configuration
 	- radio.rxEnablePipe(RX_PIPE);
 	- radio.rxConfigurePipe(RX_PIPE, Address, true, true)

- Testing for address functions
	- test_rxSetPipeAddress() etc.
*/


namespace NRF24
{
	enum DataPipe
	{
		P0,
		P1,
		P2,
		P3,
		P4,
		P5,
	};

	/**  
	* RX/TX Address field width. 
	* '00' - Illegal
	* '01' - 3 bytes 
	* '10' - 4 bytes 
	* '11' – 5 bytes
	* LSByte is used if address width is below 5 bytes 
	**/
	enum AddressWidth
	{
	    Width3Bytes = 0b01,
	    Width4Bytes = 0b10,
	    Width5Bytes = 0b11
	};

	class Driver_Base
	{
	public:
		Driver_Base();
		~Driver_Base();

		void init();
		void softReset();

		void powerOn();

		void rxEnablePipe(DataPipe pipe);
		void rxDisablePipe(DataPipe pipe);

		void rxEnableAck(DataPipe pipe);
		void rxDisableAck(DataPipe pipe);

		void setAddressWidth(AddressWidth width);
		AddressWidth getAddressWidth();

		void rxSetPipeAddress(DataPipe pipe, uint8_t* address, int size);
		void rxSetPipeAddress(DataPipe pipe, uint64_t address);
		void txSetAddress(uint64_t address);

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

		void printPrettyConfig();
		void printPrettyStatus();
		void printPrettyRxAdresses();

	protected:
		int CSN_PIN;
		int CE_PIN;

		// virtual functions to be implimented by the target platform

		virtual void SPI_BeginTransaction() = 0;
		virtual void SPI_EndTransaction() = 0;
		virtual uint8_t SPI_Transfer(uint8_t data) = 0;

		virtual void setCSN(bool state) = 0;
		virtual void setCE(bool state) = 0;

		virtual void delayMicro(unsigned int microSeconds) = 0;
		virtual void print(const char* chars) = 0;
		virtual void print(int val) = 0;
		virtual void printHex(int val) = 0; 

	public:
		uint8_t m_ReadRegister(uint8_t reg);
		void m_ReadMultiByteRegister(uint8_t reg, uint8_t* bytes, int size);

		void m_WriteRegister(uint8_t reg, uint8_t value);
		void m_WriteMultiByteRegister(uint8_t reg, uint8_t* bytes, int size);

		// TODO: I think this is stupid, will find a better way
		friend void test_m_ReadRegister();
	    friend void test_m_WriteRegister();
		friend void test_m_ReadMultiByteRegister();
		friend void test_m_WriteMultiByteRegister();
		friend void test_transmit();
	};
}


