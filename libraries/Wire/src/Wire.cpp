/*
 * TWI/I2C library for Arduino Zero
 */

#include <Arduino.h>
#include "Wire.h"

#define DEBUG_I2C 
//DBG

#define IIC_TYPE (i2c_port == 1)

#define SLC0 PINNAME_CTS
#define SDA0 PINNAME_RTS

TwoWire::TwoWire(uint8_t port, uint8_t address, u32 brg)
{
	if (brg == 400) // work around
		brg += 1;
	i2c_port = port;
	slaveAddress = address;
	i2c_speed = brg;
	transmissionBegun = false;
}

void TwoWire::begin(void)
{
	int res;
	if (1 == i2c_port)
	{
		res = Ql_IIC_Init(i2c_port, PINNAME_RI, PINNAME_DCD, IIC_TYPE);
		DEBUG_I2C("[I2C] Ql_IIC_Init( %d ) HW: %d\n", i2c_port, res);
	}
	else
	{
		res = Ql_IIC_Init(i2c_port, SLC0, SDA0, IIC_TYPE);
		DEBUG_I2C("[I2C] Ql_IIC_Init( %d ) SW: %d\n", i2c_port, res);
	}
	res = Ql_IIC_Config(i2c_port, 1, slaveAddress, i2c_speed); // set default speed
	DEBUG_I2C("[I2C] Ql_IIC_Config( %d ): %d\n", i2c_port, res);
}

void TwoWire::setClock(uint32_t brg)
{
	if (brg == 400) // work around
		brg += 1;
	i2c_speed = brg;
	int res = Ql_IIC_Config(i2c_port, 1, slaveAddress, brg);
	DEBUG_I2C("[I2C] setClock( %d ) %d, %d\n", i2c_port, res, brg);
	return;
}

void TwoWire::end()
{
	Ql_IIC_Uninit(i2c_port);
}

uint8_t TwoWire::requestFrom(uint8_t address, size_t quantity, bool stopBit)
{
	int res = -1;
	if (quantity == 0)
		return 0;
	if (!stopBit)
		return 0;
	rx.clear();
	//TODO: if IICtype = 1 ,1 < len < 8 . because our IIC contronller at most support 8 bytes
	res = Ql_IIC_Read(i2c_port, address, (uint8_t *)(rx._aucBuffer), (uint32_t)quantity);
	if (res < 0)
	{
		DEBUG_I2C("[I2C] R( %02X ) %d\n", (int)address, res);
		quantity = 0;
	}
	rx._iHead = quantity;
	return rx.available();
}

uint8_t TwoWire::requestFrom(uint8_t address, size_t quantity)
{
	return requestFrom(address, quantity, true);
}

void TwoWire::beginTransmission(uint8_t address)
{
	slaveAddress = address;
	tx.clear();
	transmissionBegun = true;
}

uint8_t TwoWire::endTransmission(bool stopBit)
{
	if (!stopBit)
		return 1;
	u32 count, i, size;
	u8 buf[8];
	transmissionBegun = false;
	// if IICtype = 1 ,1 < len < 8 . because our IIC contronller at most support 8 bytes for one time transaction
	while (tx.available() > 0)
	{
		size = tx.available();
		for (i = 0, count = 0; i < size; i++)
		{
			if (i > 7)
				break;
			buf[i] = tx.read_char();
			count++;
		}
		int res = Ql_IIC_Write(i2c_port, slaveAddress, buf, count);
		if (res < 0)
		{
			DEBUG_I2C("[I2C] ERROR Write %d\n", res);
			return 1;
		}
	}
	return 0;
}

uint8_t TwoWire::endTransmission()
{
	return endTransmission(true);
}

size_t TwoWire::write(uint8_t ucData)
{
	if (!transmissionBegun || tx.isFull())
		return 0;
	tx.store_char(ucData);
	return 1;
}

size_t TwoWire::write(const uint8_t *data, size_t quantity)
{
	for (size_t i = 0; i < quantity; ++i)
	{
		if (!write(data[i]))
			return i;
	}
	return quantity;
}

int TwoWire::available(void)
{
	return rx.available();
}

int TwoWire::read(void)
{
	return rx.read_char();
}

int TwoWire::peek(void)
{
	return rx.peek();
}

void TwoWire::flush(void)
{
	// Do nothing, use endTransmission(..) to force data transfer.
}
