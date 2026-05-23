#include "NRF24.hpp"
#include "NRF24_Utility.hpp"

NRF24_Driver_Base::NRF24_Driver_Base(/* args */)
{

}

NRF24_Driver_Base::~NRF24_Driver_Base()
{

}

void NRF24_Driver_Base::init()
{
    setCE(0);
    setCSN(1); // Deselect chip
    softReset();
    flushRx();
    flushTx();
}

void NRF24_Driver_Base::powerOn()
{
    uint8_t config = m_ReadRegister(CONFIG);
    bitSet(config, CONFIG_PWR_UP);
    m_WriteRegister(CONFIG, config);
    delayMicro(5e3); // 5ms
}

void NRF24_Driver_Base::setPayloadWidth()
{

}

void NRF24_Driver_Base::SetModeReceive()
{
    uint8_t config = m_ReadRegister(CONFIG);
    bitSet(config, CONFIG_PRIM_RX);
    m_WriteRegister(CONFIG, config);
}

// Writes a payload to the TX FIFO
void NRF24_Driver_Base::writePayload(uint8_t* data, int size)
{
    SPI_BeginTransaction();
    setCSN(0); // Active Low

    SPI_Transfer(W_TX_PAYLOAD);

    for (int i = 0; i < size; i++)
    {
        SPI_Transfer(data[i]);
    }

    setCSN(1); 
    SPI_EndTransaction();
}

void NRF24_Driver_Base::readPayload(uint8_t* data, int size)
{
    SPI_BeginTransaction();
    setCSN(0); // Active Low

    SPI_Transfer(R_RX_PAYLOAD);

    for (int i = 0; i < size; i++)
    {
        data[i] = SPI_Transfer(0x00);
    }

    setCSN(1); 
    SPI_EndTransaction();

    uint8_t status = m_ReadRegister(STATUS);
    uint8_t rx_p_no = (status >> STATUS_RX_P_NO) & 0b111; // Extract bits 3-1
    if (rx_p_no == 0b111)
    {
        bitSet(status, STATUS_RX_DR);
        m_WriteRegister(STATUS, status);
    }
}

// Checks if the TX FIFO is full (Stores up to 3 packets)
bool NRF24_Driver_Base::txFull()
{
    uint8_t status = m_ReadRegister(STATUS);
    if (bitRead(status, STATUS_TX_FULL)) return true;
    return false;
}

bool NRF24_Driver_Base::txEmpty()
{
    uint8_t fifo_status = m_ReadRegister(FIFO_STATUS);
    if (bitRead(fifo_status, FIFO_STATUS_TX_EMPTY)) return true; 
    return false;
}

bool NRF24_Driver_Base::rxDataReady()
{
    uint8_t status = m_ReadRegister(STATUS);
    bool dataReady = bitRead(status, STATUS_RX_DR);
    return dataReady;
}

bool NRF24_Driver_Base::txTransmit()
{
    // Send pending packets
	setCE(1);
	delayMicro(10);
	setCE(0);

	while (true)
	{
		uint8_t status = m_ReadRegister(STATUS);
		if (bitRead(status, STATUS_MAX_RT))
		{
			m_WriteRegister(STATUS, (1UL << STATUS_MAX_RT)); // Clear MAX_RT bit
			print("MAX RETRANSMIT REACHED\n");
			return false;
		}
		else if (bitRead(status, STATUS_TX_DS))
		{
			m_WriteRegister(STATUS, (1UL << STATUS_TX_DS)); // Clear TX_DS bit
			print("PACKET SENT\n");
			return true;
		}
	}
}

void NRF24_Driver_Base::flushTx()
{
    SPI_BeginTransaction();
    setCSN(0); // Active Low

    SPI_Transfer(FLUSH_TX);

    setCSN(1); 
    SPI_EndTransaction();
}

void NRF24_Driver_Base::flushRx()
{
    SPI_BeginTransaction();
    setCSN(0); // Active Low

    SPI_Transfer(FLUSH_RX);

    setCSN(1); 
    SPI_EndTransaction();
}

void NRF24_Driver_Base::softReset()
{
    m_WriteRegister(CONFIG,     CONFIG_RESET_VAL);
    m_WriteRegister(EN_AA,      EN_AA_RESET_VAL);
    m_WriteRegister(EN_RXADDR,  EN_RXADDR_RESET_VAL);
    m_WriteRegister(SETUP_AW,   SETUP_AW_RESET_VAL);
    m_WriteRegister(SETUP_RETR, SETUP_RETR_RESET_VAL);
    m_WriteRegister(RF_CH,      RF_CH_RESET_VAL);
    m_WriteRegister(RF_SETUP,   RF_SETUP_RESET_VAL);
    m_WriteRegister(STATUS,     STATUS_RESET_VAL);
    m_WriteRegister(RPD,        RPD_RESET_VAL);

    // TODO: RESET RX_ADDR_P0 - RX_ADDR_P5 & TX_ADDR

    // Reset RX_PW_P0 - RX_PW_P5
    for (int i = 0; i < 6; i++)
    {
        uint8_t reg_addr = RX_PW_P0 + i;
        m_WriteRegister(reg_addr, 0x00); // Reset value is 0x00
    }

    m_WriteRegister(DYNPD, DYNPD_RESET_VAL);
}

// Returns the contents of a 1 byte register
uint8_t NRF24_Driver_Base::m_ReadRegister(uint8_t reg)
{
    SPI_BeginTransaction();
    setCSN(0); // Active Low

    SPI_Transfer(R_REGISTER | reg); // SPI command or'ed with target register address
    uint8_t data = SPI_Transfer(0x00);

    setCSN(1); 
    SPI_EndTransaction();

  	return data;
}

void NRF24_Driver_Base::m_ReadMultiByteRegister(uint8_t reg, uint8_t *bytes, int size)
{
    SPI_BeginTransaction();
    setCSN(0); // Active Low

    SPI_Transfer(R_REGISTER | reg); // SPI command or'ed with target register address

    for (int i = 0; i < size; i++)
    {
        bytes[i] = SPI_Transfer(0x00);
    }

    setCSN(1); 
    SPI_EndTransaction();
}

// Writes a 8bit value to a register
void NRF24_Driver_Base::m_WriteRegister(uint8_t reg, uint8_t value)
{
    SPI_BeginTransaction();
    setCSN(0); // Active Low

    SPI_Transfer(W_REGISTER | reg); // SPI command or'ed with target register address
    SPI_Transfer(value);

    setCSN(1);
    SPI_EndTransaction();
}

void NRF24_Driver_Base::m_WriteMultiByteRegister(uint8_t reg, uint8_t *bytes, int size)
{
    SPI_BeginTransaction();
    setCSN(0); // Active Low

    SPI_Transfer(W_REGISTER | reg); // SPI command or'ed with target register address

    for (int i = 0; i < size; i++)
    {
        SPI_Transfer(bytes[i]);
    }

    setCSN(1);
    SPI_EndTransaction();
}

// =====================================
// Pretty print functions for debugging
// =====================================

void NRF24_Driver_Base::printPrettyConfig()
{
    uint8_t config = m_ReadRegister(CONFIG);

    print("==== CONFIG ====\n");

    print("MODE:       ");
    if (bitRead(config, CONFIG_PRIM_RX)) print("Receiver\n");
	else print("Transmitter\n");

    print("PWR:        ");
	if (bitRead(config, CONFIG_PWR_UP)) print("Powered up\n");
	else print("Powered down\n");
    
    print("CRC MODE:   ");
	if (bitRead(config, CONFIG_CRCO)) print("2-byte CRC\n");
	else print("1-byte CRC\n");

    print("CRC:        ");
	if (bitRead(config, CONFIG_EN_CRC)) print("Enabled\n");
	else print("Disabled\n");

    print("MAX_RT IQR: ");
	if (bitRead(config, CONFIG_MASK_MAX_RT)) print("Enabled\n");
	else print("Disabled\n");

    print("TX_DS IQR:  ");
	if (bitRead(config, CONFIG_MASK_TX_DS)) print("Enabled\n");
	else print("Disabled\n");

    print("RX_DR IQR:  ");
	if (bitRead(config, CONFIG_MASK_RX_DR)) print("Enabled\n");
	else print("Disabled\n");
}

void NRF24_Driver_Base::printPrettyStatus()
{
    uint8_t status  = m_ReadRegister(STATUS);
  	print("==== STATUS ====\n");

  	print("RX_DR:   ");
  	print(bitRead(status, STATUS_RX_DR));
    print("\n");

  	print("TX_DS:   ");
  	print(bitRead(status, STATUS_TX_DS));
    print("\n");

  	print("MAX_RT:  ");
  	print(bitRead(status, STATUS_MAX_RT));
    print("\n");

  	print("RX_P_NO: ");
  	uint8_t rx_p_no = (status >> STATUS_RX_P_NO) & 0b111; // Extract bits 3-1
  	switch (rx_p_no)
  	{
  	case 0b110:
		print("UNUSED\n");
  	  	break;
  	case 0b111:
  	  	print("RX FIFO Empty\n");
  	  	break;
	
  	default:
		print("Data ready in RX pipe: ");
		print(rx_p_no);
        print("\n");
  	  	break;
	}
    
	print("TX_FULL: ");
  	print(bitRead(status, STATUS_TX_FULL));
    print("\n");
}

void NRF24_Driver_Base::printPrettyRxAdresses()
{
    uint8_t rx_addr_p0[5];
    uint8_t rx_addr_p1[5];
    uint8_t rx_addr_p2;
    uint8_t rx_addr_p3;
    uint8_t rx_addr_p4;
    uint8_t rx_addr_p5;

    m_ReadMultiByteRegister(RX_ADDR_P0, rx_addr_p0, 5);
    m_ReadMultiByteRegister(RX_ADDR_P1, rx_addr_p1, 5);
    rx_addr_p2 = m_ReadRegister(RX_ADDR_P2);
    rx_addr_p3 = m_ReadRegister(RX_ADDR_P3);
    rx_addr_p4 = m_ReadRegister(RX_ADDR_P4);
    rx_addr_p5 = m_ReadRegister(RX_ADDR_P5);

    print("==== RX ADRESSES ====\n");
    
    print("P0: 0x");
    
    for (int i = 4; i >= 0; i--)
        printHex(rx_addr_p0[i]);
    print("\n");

    print("P1: 0x");
    for (int i = 4; i >= 0; i--)
        printHex(rx_addr_p1[i]);
    print("\n");

    print("P2: 0x");
    for (int i = 4; i >= 1; i--)
        printHex(rx_addr_p1[i]);
    printHex(rx_addr_p2);
    print("\n");

    print("P3: 0x");
    for (int i = 4; i >= 1; i--)
        printHex(rx_addr_p1[i]);
    printHex(rx_addr_p3);
    print("\n");

    print("P4: 0x");
    for (int i = 4; i >= 1; i--)
        printHex(rx_addr_p1[i]);
    printHex(rx_addr_p4);
    print("\n");

    print("P5: 0x");
    for (int i = 4; i >= 1; i--)
        printHex(rx_addr_p1[i]);
    printHex(rx_addr_p5);
    print("\n");
}
