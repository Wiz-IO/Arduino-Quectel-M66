/*  for Quectel
 *  Created on: 01.01.2019
 *      Author: Georgi Angelov
 */

#include "EEPROM.h"

#ifdef __OCPU_RIL_SUPPORT__

EEPROMClass EEPROM;

// cell from 1 to 13
static void ql_eeprom_get_index(u32 address, u8 *cell, u32 *size, u32 *offset)
{
    if (address < 400)
    {
        *cell = 1 + (address / 50);
        *size = 50;
        *offset = 0;
        return;
    }
    else if (address < 800)
    {
        address -= 400;
        *cell = 9 + (address / 100);
        *size = 100;
        *offset = 400;
        return;
    }
    else
    {
        *cell = 13;
        *size = 500;
        *offset = 800;
    }
}

uint8_t EERef::operator*() const
{
    if (index < _EEPROM_SIZE)
    {
        u8 cell;
        u32 size, offset;
        ql_eeprom_get_index(index, &cell, &size, &offset);
        uint8_t data[size];
        int res = Ql_SecureData_Read(cell, (u8 *)data, size); // RIL
        DEBUG_EEP("Read cell = %d, size = %d, res = %d", cell, size, res);
        if (size == res)
        {
            return data[index - offset];
        }
        else
        {
            DEBUG_EEP("Read failed. Return 0 as value");
        }
    }
    return 0;
}

EERef &EERef::operator=(uint8_t in)
{
    if (index < _EEPROM_SIZE)
    {
        u8 cell;
        u32 size, offset;
        ql_eeprom_get_index(index, &cell, &size, &offset);
        uint8_t data[size];
        int res = Ql_SecureData_Read(cell, (u8 *)data, size); // RIL
        DEBUG_EEP("Write cell = %d, size = %d, res = %d", cell, size, res);
        if (size == res)
        {
            data[index - offset] = in;
            Ql_SecureData_Store(cell, (u8 *)data, size); // RIL
        }
        else
        {
            DEBUG_EEP("Write failed ( %d )", res);
        }
    }
    return *this;
}

#endif
