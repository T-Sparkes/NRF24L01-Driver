#include <Arduino.h>
#include <SPI.h>

//#include "NRF24.hpp"
#include "NRF24_Arduino.hpp"
#include "NRF24_Utility.hpp"

#define NRF24_TX
#define NRF24_CE_PIN 7
#define NRF24_CSN_PIN 8
NRF24_Arduino radio(NRF24_CSN_PIN, NRF24_CE_PIN);

struct testPacket
{
	uint64_t id;
	char data[24];
};

// TX PROGRAM
#ifdef NRF24_TX
void setup() 
{
	SPI.begin();
	Serial.begin(115200);
	delay(1000);

	// TX SETUP
	radio.init();
	radio.printPrettyConfig();
	radio.printPrettyRxAdresses();

	radio.powerOn();
	radio.setAddressWidth(NRF24_Arduino::Width5Bytes);
	radio.rxDisableAck(NRF24_Arduino::P0);
	
	radio.txSetAddress(0xAABBCCDDEE); // Set TX address, must be the same as RX address for pipe 0 to use ACK
	radio.rxSetPipeAddress(NRF24_Arduino::P0, 0xAABBCCDDEE);

	radio.printPrettyRxAdresses();
}

void loop()
{
	static uint64_t counter = 0;

	testPacket test;
	test.id = counter++;
	strcpy(test.data, "Test Data");

	radio.writePayload((uint8_t*)&test, sizeof(test));
	radio.txTransmit();
	
	// Transmit continously for 4ms, then rest for a few micros (EXPERIMENT);
	/*
	unsigned long startTime = micros();
	radio.setCE(1); // continues transmission
	while (micros() - startTime < 4e3) // becomes unstable after ~ 4ms
	{
		testPacket test;
		test.id = counter++;
		strcpy(test.data, "Test Data");
		radio.writePayload((uint8_t*)&test, sizeof(test));
	}
	radio.setCE(0); // Stop transmission
	*/
}
#endif

// RX PROGRAM
#ifdef NRF24_RX
void setup() 
{
	SPI.begin();
	Serial.begin(115200);
	delay(1000);

	radio.init();

	radio.powerOn();
	radio.setAddressWidth(NRF24_Arduino::Width5Bytes);
	radio.rxDisableAck(NRF24_Arduino::P0);

	radio.rxSetPipeAddress(NRF24_Arduino::P0, 0xAABBCCDDEE);

	radio.SetModeReceive(); // Set mode 
	radio.m_WriteRegister(RX_PW_P0, sizeof(testPacket)); // Set payload size (TEMP)
	
	radio.setCE(1); // Enable Receiving 

	radio.printPrettyConfig();
	radio.printPrettyStatus();
	radio.printPrettyRxAdresses();
}

void loop() 
{
	static unsigned long bytesReceived = 0;
	static unsigned long startTime = millis();

	// Calculate bytes per second
	while (radio.rxDataReady())
	{
		testPacket packet;
		radio.readPayload(reinterpret_cast<uint8_t*>(&packet), sizeof(testPacket));

		//Serial.print("Received: ");
		//Serial.print(packet.data);
		//Serial.print(" with id: ");
		//Serial.println((unsigned long)packet.id);
		bytesReceived += sizeof(testPacket);
	}
	//delay(1000);

	unsigned long elapsedTime = millis() - startTime;
	if (elapsedTime >= 1000)
	{
		float Bps = (bytesReceived) / (elapsedTime / 1000.0);
		Serial.print("Data Rate: ");
		Serial.print(Bps / 1000.0);
		Serial.println(" kB/s");
		bytesReceived = 0;
		startTime = millis();
	}
}
#endif