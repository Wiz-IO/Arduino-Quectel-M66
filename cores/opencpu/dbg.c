/*
 * debug.c
 *
 *  Created on: 26.02.2019
 *      Author: georgi.angelov
 */

#include "ql_stdlib.h"
#include "ql_uart.h"
#include "dbg.h"
#include "api.h"

static Enum_SerialPort debugPort = UART_PORT_END;
static u32 debugMutex = 0;

void debug_enable(unsigned long port)
{
	if (0 == debugMutex)
		debugMutex = Ql_OS_CreateMutex("DEBUG_MUTEX");
	debugPort = (Enum_SerialPort)port;
}

void debug_disable(void)
{
	debugPort = (Enum_SerialPort)UART_PORT_END;
}

#ifdef ENABLE_DEBUG

static char bufferDebug[1024];

int log_printf(const char *frm, ...)
{
	if (0 == debugMutex)
		return 0;
	va_list arg;
	va_start(arg, frm);
	if (debugPort < UART_PORT1 || debugPort > UART_PORT3)
		return 0;
	if (debugMutex)
		Ql_OS_TakeMutex(debugMutex);
	int n = wiz__vsnprintf(bufferDebug, sizeof(bufferDebug), frm, arg);
	va_end(arg);
	if (n > 0)
		n = Ql_UART_Write(debugPort, (u8 *)bufferDebug, n);
	if (debugMutex)
		Ql_OS_GiveMutex(debugMutex);
	return n;
}

void log_buf(const char *text, const unsigned char *buf, unsigned int len)
{
	char txt[17];
	char bufferDump[512];
	unsigned int i, idx = 0;
	Ql_snprintf(bufferDump + idx, sizeof(bufferDump) - idx, "[DMP] '%s' (%u bytes)\n", text, (unsigned int)len);
	log_printf(bufferDump);
	idx = 0;
	Ql_memset(txt, 0, sizeof(txt));
	for (i = 0; i < len; i++)
	{
		if (i >= 4096)
			break;
		if (i % 16 == 0)
		{
			if (i > 0)
			{
				Ql_snprintf(bufferDump + idx, sizeof(bufferDump) - idx, "  %s\n", txt);
				log_printf(bufferDump);
				idx = 0;
				Ql_memset(txt, 0, sizeof(txt));
			}
			idx += Ql_snprintf(bufferDump + idx, sizeof(bufferDump) - idx, "%04X: ", (unsigned int)i);
		}
		idx += Ql_snprintf(bufferDump + idx, sizeof(bufferDump) - idx, " %02X", (unsigned int)buf[i]);
		txt[i % 16] = (buf[i] > 31 && buf[i] < 127) ? buf[i] : '.';
	}
	if (len > 0)
	{
		for (/* i = i */; i % 16 != 0; i++)
			idx += Ql_snprintf(bufferDump + idx, sizeof(bufferDump) - idx, "   ");
		Ql_snprintf(bufferDump + idx, sizeof(bufferDump) - idx, "  %s\n", txt);
		log_printf(bufferDump);
	}
}

#endif