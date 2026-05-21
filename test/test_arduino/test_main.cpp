#include <Arduino.h>
#include <unity.h>

#include "NRF24_Arduino.hpp"

#define NRF24_CE_PIN 7
#define NRF24_CSN_PIN 8
NRF24_Arduino radio(NRF24_CSN_PIN, NRF24_CE_PIN);

void test_m_ReadRegister()
{
  uint8_t config = radio.m_ReadRegister(CONFIG);
  TEST_ASSERT_EQUAL(CONFIG_RESET_VAL, config);
}

void test_m_ReadMultiByteRegister()
{
  uint8_t bytes[5];
  radio.m_ReadMultiByteRegister(RX_ADDR_P1, bytes, 5);
  
  for (int i = 0; i < 5; i++)
  {
    TEST_ASSERT_EQUAL(0xC2, bytes[i]);
  }  
}

void test_m_WriteRegister()
{
  radio.m_WriteRegister(RX_ADDR_P2, 0xAA);
  uint8_t addr = radio.m_ReadRegister(RX_ADDR_P2);
  TEST_ASSERT_EQUAL(0xAA, addr);
}

void setup()
{
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  delay(2000);
  SPI.begin();
  radio.init();

  UNITY_BEGIN(); // IMPORTANT LINE!

  RUN_TEST(test_m_ReadRegister);
  RUN_TEST(test_m_ReadMultiByteRegister);
  RUN_TEST(test_m_WriteRegister);
}

void loop()
{
  UNITY_END(); // stop unit testing
}