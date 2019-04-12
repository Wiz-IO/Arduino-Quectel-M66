#ifndef _AT_TOK_H_
#define _AT_TOK_H_

#define __OCPU_AT_TOK_SUPPORT__

/*
 * android_hardware_ril /reference-ril/at_tok.c
 * https://searchcode.com/codesearch/view/40550194/
 */

#include "ql_type.h"
#include "ql_stdlib.h"
#include "ql_system.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#define CHR_DELIM   ':'
#define CHR_QUOTES  '"'
#define STR_QUOTES  "\""

int isspace(int c);
char * strpbrk(const char *s1, const char *s2);
char * strsep(char **stringp, const char *delim);
void skipWhiteSpace(char **p_cur);
char * nextTok(char **p_cur);

int at_tok_start(char **p_cur);
int at_tok_nextint(char **p_cur, int *p_out);
int at_tok_nexthexint(char **p_cur, int *p_out);
int at_tok_nextbool(char **p_cur, char *p_out);
int at_tok_nextstr(char **p_cur, char **out);
int at_tok_hasmore(char **p_cur);

#endif /* _AT_TOK_H_ */
