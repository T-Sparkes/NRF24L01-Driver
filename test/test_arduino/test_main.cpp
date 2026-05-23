#include <Arduino.h>
#include <unity.h>

#include "NRF24_Arduino.hpp"
#include "NRF24_Utility.hpp"

#define NRF24_CE_PIN 7
#define NRF24_CSN_PIN 8
NRF24_Arduino radio(NRF24_CSN_PIN, NRF24_CE_PIN);

// TODO: Add setup and tear down so each test starts and ends in a known state

void setUp(void) 
{
    // set stuff up here
	radio.init();
}

void tearDown(void) 
{
    // clean stuff up here
	radio.softReset();
}

void test_m_ReadRegister()
{
	uint8_t config = radio.m_ReadRegister(CONFIG);
	TEST_ASSERT_EQUAL_HEX8(CONFIG_RESET_VAL, config);
}

void test_m_ReadMultiByteRegister()
{
	uint8_t expected[] = {0xC2, 0xC2, 0xC2, 0xC2, 0xC2}; // Reset Address is 0xC2C2C2C2C2
	uint8_t bytes[5];

	radio.m_ReadMultiByteRegister(RX_ADDR_P1, bytes, 5);
	TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, bytes, 5); 
}

void test_m_WriteRegister()
{
	radio.m_WriteRegister(RX_ADDR_P2, 0xAA);
	uint8_t addr = radio.m_ReadRegister(RX_ADDR_P2);
	TEST_ASSERT_EQUAL_HEX8(0xAA, addr);
}

void test_m_WriteMultiByteRegister()
{
	uint8_t expected[] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA}; // Reset Address is 0xC2C2C2C2C2
	uint8_t bytes[5];

	radio.m_WriteMultiByteRegister(RX_ADDR_P1, expected, 5);
	radio.m_ReadMultiByteRegister(RX_ADDR_P1, bytes, 5);
	TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, bytes, 5); 
}

void test_writePayload()
{
	uint8_t byte = 0xAA;

	TEST_ASSERT_FALSE(radio.txFull());

	// FIFO holds 3 payloads, max payload size 32 bytes
	radio.writePayload(&byte, sizeof(byte));
	radio.writePayload(&byte, sizeof(byte));
	radio.writePayload(&byte, sizeof(byte));

	TEST_ASSERT_TRUE(radio.txFull());

	radio.flushTx();
	TEST_ASSERT_FALSE(radio.txFull());
}

void test_transmit()
{
	uint8_t byte = 0xAA;
	radio.powerOn();
	radio.writePayload(&byte, sizeof(byte));
	TEST_ASSERT_FALSE(radio.txTransmit()); // Auto ack on, TX Fails

	radio.m_WriteRegister(EN_AA, 0x00); // Disable auto ack
	TEST_ASSERT_TRUE(radio.txTransmit()); // No ack TX Succeds
}

void setup()
{
	delay(1000);
	SPI.begin();

	UNITY_BEGIN(); 

	RUN_TEST(test_m_ReadRegister);
	RUN_TEST(test_m_ReadMultiByteRegister);
	RUN_TEST(test_m_WriteRegister);
	RUN_TEST(test_m_WriteMultiByteRegister);
	RUN_TEST(test_writePayload);
	RUN_TEST(test_transmit);
}

void loop()
{
	UNITY_END();
}