/*
 * debug.h
 *
 *  Created on: 26.02.2019
 *      Author: georgi.angelov
 */

#ifndef DBG_H_
#define DBG_H_

#define ENABLE_DEBUG
#define ENABLE_TRACE

void debug_enable(unsigned long port);

int log_printf(const char *frm, ...)  __attribute__((weak));
void log_buf(const char *text, const unsigned char *buf, unsigned int len)  __attribute__((weak));

#ifdef ENABLE_DEBUG
#   include "ql_trace.h"
#   define DBG(FORMAT, ...) log_printf(FORMAT, ##__VA_ARGS__)
#   define DMP(TEXT, BUFFER, LEN) log_buf(TEXT, (unsigned char *)BUFFER, LEN)
#else
#   define DBG(FORMAT, ...) 
#   define DMP(TEXT, BUFFER, LEN)
#endif


#ifdef ENABLE_TRACE
#   define TRACE Ql_Debug_Trace
#else
#   define TRACE
#endif

#endif /* DBG_H_ */
