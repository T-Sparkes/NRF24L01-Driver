#include <Arduino.h>
#include <SPI.h>

#include "NRF24.hpp"
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
	radio.m_WriteRegister(EN_AA, 0x00); // Disable auto ack (TEMP)

	uint64_t address = 0xAABBCCDDEE;
	radio.setAddressWidth(Width5Bytes);
	radio.rxSetPipeAddress(RX_P0, address); // Set RX address for pipe 0
	radio.txSetAddress(address); // Set TX address, must be the same as RX address for pipe 0 to use ACK
	radio.rxSetPipeAddress(RX_P1, 0xFFFFFFFFC2);
	radio.printPrettyRxAdresses();
}

void loop()
{
	static uint64_t counter = 0;
	//radio.printPrettyConfig();
	//radio.printPrettyStatus();

	testPacket test;
	test.id = counter++;
	strcpy(test.data, "Test Data");

	radio.writePayload((uint8_t*)&test, sizeof(test));
	//radio.txTransmit();
	
	// Transmit continously for 4ms, then rest for a few micros;
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
	Serial.begin(250000);
	delay(1000);

	radio.init();

	uint8_t address[] = {0xEE, 0xDD, 0xCC, 0xBB, 0xAA};
	radio.m_WriteMultiByteRegister(RX_ADDR_P0, address, 5); // Set RX address for pipe 0

	radio.m_WriteRegister(EN_AA, 0x00); // Disable auto ack
	radio.SetModeReceive(); // Set mode 
	radio.m_WriteRegister(RX_PW_P0, sizeof(testPacket)); // Set payload size
	
	radio.powerOn();
	radio.setCE(1); // Enable Receiving 

	radio.printPrettyConfig();
	radio.printPrettyStatus();
	radio.printPrettyRxAdresses();
}

void loop() 
{
	static unsigned long bytesReceived = 0;
	static unsigned long startTime = millis();
	//radio.printPrettyStatus();
	//radio.printPrettyConfig();
	//delay(1000);

	// Calculate bytes per second
	while (radio.rxDataReady())
	{
		testPacket packet;
		radio.readPayload((uint8_t*)&packet, sizeof(testPacket));

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
		Serial.println(" kB/s");..
		bytesReceived = 0;
		startTime = millis();
	}
}
#endif