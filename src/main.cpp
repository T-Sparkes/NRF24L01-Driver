#include <Arduino.h>
#include <SPI.h>

#include "NRF24.hpp"
#include "NRF24_Arduino.hpp"

#define NRF24_CE_PIN 7
#define NRF24_CSN_PIN 8

NRF24_Arduino radio(NRF24_CSN_PIN, NRF24_CE_PIN);

void txSetup();
void txLoop();
void rxSetup();
void rxLoop();

void setup() 
{
	SPI.begin();
	Serial.begin(250000);
	delay(1000);

	txSetup();
	//rxSetup();	
}

void loop() 
{
	txLoop();
	//rxLoop();
}

void txSetup()
{
	radio.init();
	radio.softReset();
	radio.printPrettyConfig();

	radio.powerOn();
	//radio.m_WriteRegister(EN_AA, 0x00); // Disable auto ack
}

void txLoop()
{
	//radio.printPrettyConfig();
	//radio.printPrettyStatus();

	if (!radio.txFull())
	{
		char test[] = "Hello World!"; 
		radio.writePayload((uint8_t*)&test, sizeof(test));
	}

	// Send pending packets
	radio.setCE(1);
	delayMicroseconds(10);
	radio.setCE(0);

	while (true)
	{
		uint8_t status = radio.m_ReadRegister(STATUS);
		if (bitRead(status, STATUS_MAX_RT))
		{
			radio.m_WriteRegister(STATUS, (1UL << STATUS_MAX_RT));
			Serial.println("MAX RETRANSMIT REACHED");
			break;
		}
		else if (bitRead(status, STATUS_TX_DS))
		{
			radio.m_WriteRegister(STATUS, (1UL << STATUS_TX_DS));
			Serial.println("PACKET SENT");
			break;
		}
	}
	delay(1000);
}

void rxSetup()
{
	radio.init();
	radio.softReset();
	radio.printPrettyConfig();

	radio.powerOn();
	//radio.m_WriteRegister(EN_AA, 0x00); // Disable auto ack
	radio.SetModeReceive(); // Set mode 
	radio.m_WriteRegister(RX_PW_P0, 13); // Set payload size
	radio.setCE(1); // Enable Receiving 
}

void rxLoop()
{
	//radio.printPrettyStatus();
	//radio.printPrettyConfig();
	//delay(1000);

	while (radio.rxDataReady())
	{
		char data[13];
		radio.readPayload((uint8_t*)&data, 13);

		Serial.print("Received: ");
		Serial.println(data);
	}
	//delay(1000);
}