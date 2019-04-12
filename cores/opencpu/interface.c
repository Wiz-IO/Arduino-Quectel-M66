/*
 * interface.c M66
 *
 *  Created on: 11.07.2018
 *      Author: Georgi Angelov
 */

#include "interface.h"

/* CPP INIT */
extern void (*__preinit_array_start[])(void) __attribute__((weak));
extern void (*__preinit_array_end[])(void) __attribute__((weak));
extern void (*__init_array_start[])(void) __attribute__((weak));
extern void (*__init_array_end[])(void) __attribute__((weak));
extern void (*__fini_array_start[])(void) __attribute__((weak));
extern void (*__fini_array_end[])(void) __attribute__((weak));

extern void _init(void) __attribute__((weak));
extern void _fini(void) __attribute__((weak));

void __libc_init_array(void);
void __libc_fini_array(void);

void __libc_init_array(void)
{
	size_t count;
	size_t i;
	count = __preinit_array_end - __preinit_array_start;
	for (i = 0; i < count; i++)
		__preinit_array_start[i]();
	_init();
	count = __init_array_end - __init_array_start;
	for (i = 0; i < count; i++)
		__init_array_start[i]();
}

void __libc_fini_array(void)
{
	size_t count;
	size_t i;
	count = __fini_array_end - __fini_array_start;
	for (i = count; i > 0; i--)
		__fini_array_start[i - 1]();
	_fini();
}

inline void abort(void)
{
	while (1)
	{
	}
}

void __cxa_finalize(void *handle) {}
void __cxa_pure_virtual(void)
{
	abort();
}

void __cxa_deleted_virtual(void)
{
	abort();
}

void *realloc(void *mem, size_t newsize)
{
	if (newsize == 0)
	{
		free(mem);
		return NULL;
	}
	void *p;
	p = malloc(newsize);
	if (p)
	{
		if (mem != NULL)
		{
			memcpy(p, mem, newsize);
			free(mem);
		}
	}
	return p;
}

void *calloc(size_t count, size_t size)
{
	size_t alloc_size = count * size;
	void *new = malloc(alloc_size);
	if (new)
	{
		memset(new, 0, alloc_size);
		return new;
	}
	return NULL;
}

void *memcpy(void *dest, const void *src, size_t len)
{ // need for quectel startup
	char *d = dest;
	const char *s = src;
	while (len--)
		*d++ = *s++;
	return dest;
}

void *memset(void *dest, int val, size_t len)
{ // need for quectel startup
	unsigned char *ptr = dest;
	while (len-- > 0)
		*ptr++ = val;
	return dest;
}

void reverse(char *begin, char *end)
{
	char *is = begin;
	char *ie = end - 1;
	while (is < ie)
	{
		char tmp = *ie;
		*ie = *is;
		*is = tmp;
		++is;
		--ie;
	}
}

extern long atol(const char *s)
{
	long val = 0;
	if (s)
		Ql_sscanf(s, "%l", &val);
	return val;
}

static const char *str_digits = "0123456789abcdef";
extern char *itoa(int value, char *result, int base)
{
	if (result)
	{
		if (base < 2 || base > 16)
		{
			*result = 0;
			return result;
		}
		char *out = result;
		int quotient = abs(value);
		do
		{
			const int tmp = quotient / base;
			*out = str_digits[quotient - (tmp * base)];
			++out;
			quotient = tmp;
		} while (quotient);
		if (value < 0)
			*out++ = '-';
		reverse(result, out);
		*out = 0;
	}
	return result;
}

extern char *ltoa(long value, char *result, int base)
{
	if (result)
	{
		if (base < 2 || base > 16)
		{
			*result = 0;
			return result;
		}
		char *out = result;
		long quotient = abs(value);
		do
		{
			const long tmp = quotient / base;
			*out = str_digits[quotient - (tmp * base)];
			++out;
			quotient = tmp;
		} while (quotient);
		if (value < 0)
			*out++ = '-';
		reverse(result, out);
		*out = 0;
	}
	return result;
}

extern char *utoa(unsigned value, char *result, int base)
{
	if (result)
	{
		if (base < 2 || base > 16)
		{
			*result = 0;
			return result;
		}
		char *out = result;
		unsigned quotient = value;
		do
		{
			const unsigned tmp = quotient / base;
			*out = str_digits[quotient - (tmp * base)];
			++out;
			quotient = tmp;
		} while (quotient);
		reverse(result, out);
		*out = 0;
	}
	return result;
}

extern char *ultoa(unsigned long value, char *result, int base)
{
	if (result)
	{
		if (base < 2 || base > 16)
		{
			*result = 0;
			return result;
		}
		char *out = result;
		unsigned long quotient = value;
		do
		{
			const unsigned long tmp = quotient / base;
			*out = str_digits[quotient - (tmp * base)];
			++out;
			quotient = tmp;
		} while (quotient);
		reverse(result, out);
		*out = 0;
	}
	return result;
}

inline int isascii(int c)
{
	return c >= 0 && c < 128;
}

inline int toascii(int c)
{
	return c & 0177;
}

inline int isalpha(int c)
{
	return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

inline int isspace(int c)
{
	return (c == ' ' || c == '\t' || c == '\n' || c == '\12');
}

inline int isdigit(int c)
{
	return (c >= '0' && c <= '9');
}

inline int isxdigit(int c)
{
	return isdigit(c) || ((unsigned)c | 32) - 'a' < 6;
}

unsigned int *Ql_convertIP(unsigned int ip)
{
	static char m_ipAddress[4];
	unsigned int *p = (unsigned int *)m_ipAddress;
	*p = ip;
	return p;
}

int Ql_inet_aton(const char *cp, uint32_t *ip)
{
	if (!ip || !cp)
		return 0;
	if (0 == Ql_IpHelper_ConvertIpAddr((u8 *)cp, (u32 *)ip))
		return 1;
	return 0;
}