#pragma once

#include <stdint.h>
#define bitRead(value,bit) (((value) >> (bit)) & 0x01)
#define bitSet(value,bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))

enum SPI_COMMANDS
{
  	R_REGISTER   = 0b00000000,    
  	W_REGISTER   = 0b00100000,   
  	R_RX_PAYLOAD = 0b01100001,   
  	W_TX_PAYLOAD = 0b10100000,   
  	FLUSH_TX     = 0b11100001,   
  	FLUSH_RX     = 0b11100010,   
  	REUSE_TX_PL  = 0b11100011,   
  	NOP          = 0b11111111   
};

enum REGISTER_ADDRESSES
{
  	CONFIG     = 0x00,
  	EN_AA      = 0x01,
  	EN_RXADDR  = 0x02,
  	SETUP_AW   = 0x03,
  	SETUP_RETR = 0x04,
  	RF_CH      = 0x05,
  	RF_SETUP   = 0x06,
  	STATUS     = 0x07,
  	OBSERVE_TX = 0x08,
  	RPD        = 0x09,
  	RX_ADDR_P0 = 0x0A,
  	RX_ADDR_P1 = 0x0B,
  	RX_ADDR_P2 = 0x0C,
  	RX_ADDR_P3 = 0x0D,
  	RX_ADDR_P4 = 0x0E,
  	RX_ADDR_P5 = 0x0F,
  	TX_ADDR    = 0x10,
  	RX_PW_P0   = 0x11,
  	RX_PW_P1   = 0x12,
  	RX_PW_P2   = 0x13,
  	RX_PW_P3   = 0x14,
  	RX_PW_P4   = 0x15,
  	RX_PW_P5   = 0x16,
  	FIFO_STATUS = 0x17,
  	DYNPD       = 0x1C,
};

enum REGISTER_RESET_VALS
{
	CONFIG_RESET_VAL     = 0x08,
  	EN_AA_RESET_VAL      = 0x3F,
  	EN_RXADDR_RESET_VAL  = 0x03,
  	SETUP_AW_RESET_VAL   = 0x03,
  	SETUP_RETR_RESET_VAL = 0x03,
  	RF_CH_RESET_VAL      = 0x02,
  	RF_SETUP_RESET_VAL   = 0x0E,
  	STATUS_RESET_VAL     = 0x0E,
  	RPD_RESET_VAL        = 0x00,
  	RX_ADDR_P0_RESET_VAL = 0xE7E7E7E7E7,
  	RX_ADDR_P1_RESET_VAL = 0xC2C2C2C2C2,
  	RX_ADDR_P2_RESET_VAL = 0xC3,
  	RX_ADDR_P3_RESET_VAL = 0xC4,
  	RX_ADDR_P4_RESET_VAL = 0xC5,
  	RX_ADDR_P5_RESET_VAL = 0xC6,
  	TX_ADDR_RESET_VAL    = 0xE7E7E7E7E7,
  	RX_PW_P0_RESET_VAL   = 0x00,
  	RX_PW_P1_RESET_VAL   = 0x00,
  	RX_PW_P2_RESET_VAL   = 0x00,
  	RX_PW_P3_RESET_VAL   = 0x00,
  	RX_PW_P4_RESET_VAL   = 0x00,
  	RX_PW_P5_RESET_VAL   = 0x00,
	DYNPD_RESET_VAL       = 0x00
};

enum CONFIG_BIT_POS
{
  	CONFIG_PRIM_RX     = 0x00, 
  	CONFIG_PWR_UP      = 0x01, 
  	CONFIG_CRCO        = 0x02, 
  	CONFIG_EN_CRC      = 0x03, 
  	CONFIG_MASK_MAX_RT = 0x04, 
  	CONFIG_MASK_TX_DS  = 0x05, 
  	CONFIG_MASK_RX_DR  = 0x06, 
    CONFIG_RESERVED    = 0x07  
};

enum EN_AA_BIT_POS
{
  	EN_AA_P0 = 0x00,
  	EN_AA_P1 = 0x01,
  	EN_AA_P2 = 0x02,
  	EN_AA_P3 = 0x03,
  	EN_AA_P4 = 0x04,
  	EN_AA_P5 = 0x05
};

enum EN_RXADDR_BIT_POS
{
  	EN_RXADDR_P0 = 0x00,
  	EN_RXADDR_P1 = 0x01,
  	EN_RXADDR_P2 = 0x02,
  	EN_RXADDR_P3 = 0x03,
  	EN_RXADDR_P4 = 0x04,
  	EN_RXADDR_P5 = 0x05
};

enum SETUP_AW_BIT_POS
{
  	SETUP_AW_3BYTES = 0b01,
  	SETUP_AW_4BYTES = 0b10,
  	SETUP_AW_5BYTES = 0b11
};

enum SETUP_RETR_BIT_POS
{
  	SETUP_RETR_ARD = 0x04, // Bits 7-4
  	SETUP_RETR_ARC = 0x00  // Bits 3-0
};

enum RF_CH_BIT_POS
{
  	RF_CH_RF_CH = 0x00 // Bits 6-0
};

enum RF_SETUP_BIT_POS
{
  	RF_SETUP_CONT_WAVE   = 0x07,
  	RF_SETUP_RF_DR_LOW   = 0x05,
  	RF_SETUP_PLL_LOCK    = 0x04,
  	RF_SETUP_RF_DR_HIGH  = 0x03,
  	RF_SETUP_RF_PWR      = 0x01 // Bits 2-1
};

enum STATUS_BIT_POS
{
  	STATUS_RX_DR   = 0x06, 
  	STATUS_TX_DS   = 0x05, 
  	STATUS_MAX_RT  = 0x04, 
  	STATUS_RX_P_NO = 0x01, // Bits 3-1. 
  	STATUS_TX_FULL = 0x00  
};

enum OBSERVE_TX_BIT_POS
{
  	OBSERVE_TX_PLOS_CNT = 0x04, // Bits 7-4
  	OBSERVE_TX_ARC_CNT  = 0x00  // Bits 3-0
};

enum RPD_BIT_POS
{
  	RPD_RPD = 0x00
};

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
	bool isTxFifoFull();
	bool rxAvailable();

	void softReset();
	void printPrettyConfig();
	void printPrettyStatus();

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
	
public: // Will be private
	uint8_t m_ReadRegister(uint8_t reg);
	void m_WriteRegister(uint8_t reg, uint8_t value);
};



