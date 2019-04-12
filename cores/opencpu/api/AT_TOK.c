#include "AT_TOK.h"

char * strpbrk(const char *s1, const char *s2)
{
    register const char * scanp;
    register int c, sc;
    while ((c = *s1++) != 0)
    {
        for (scanp = s2; (sc = *scanp++) != 0;)
            if (sc == c)
                return ((char *)(s1 - 1));
    }
    return (NULL);
}

char * strsep(char **stringp, const char *delim)
{
    if (*stringp == NULL) { return NULL; }
    char *token_start = *stringp;
    *stringp = strpbrk(token_start, delim);
    if (*stringp)
    {
        **stringp = '\0';
        (*stringp)++;
    }
    return token_start;
}

int at_tok_start(char **p_cur)
{
    if (*p_cur == NULL)  return -1;
    *p_cur = Ql_strchr(*p_cur, ':');
    if (*p_cur == NULL) return -1;
    (*p_cur)++;
    return 0;
}

void skipWhiteSpace(char **p_cur)
{
    if (*p_cur == NULL) return;
    while (**p_cur != '\0' && isspace(**p_cur)) (*p_cur)++;
}

static void skipNextComma(char **p_cur)
{
    if (*p_cur == NULL) return;
    while (**p_cur != '\0' && **p_cur != ',') (*p_cur)++;
    if (**p_cur == ',') (*p_cur)++;
}

char * nextTok(char **p_cur)
{
    char *ret = NULL;
    skipWhiteSpace(p_cur);
    if (*p_cur == NULL)
    {
        ret = NULL;
    } else if (**p_cur == CHR_QUOTES)
    {
        (*p_cur)++;
        ret = strsep(p_cur, STR_QUOTES);
        skipNextComma(p_cur);
    } else {
        ret = strsep(p_cur, ",");
    }
    return ret;
}

static int at_tok_nextint_base(char **p_cur, int *p_out, int base, int  uns)
{
    char *ret;
    if (*p_cur == NULL) {
        return -1;
    }
    ret = nextTok(p_cur);
    if (ret == NULL) {
        return -1;
    } else {
        long l;
        char *end;
        if (uns)
            l = strtoul(ret, &end, base);
        else
            l = strtol(ret, &end, base);
        *p_out = (int)l;
        if (end == ret) {
            return -1;
        }
    }
    return 0;
}

int at_tok_nextint(char **p_cur, int *p_out)
{
    return at_tok_nextint_base(p_cur, p_out, 10, 0);
}

int at_tok_nexthexint(char **p_cur, int *p_out)
{
    return at_tok_nextint_base(p_cur, p_out, 16, 1);
}

int at_tok_nextbool(char **p_cur, char *p_out)
{
    int ret;
    int result;
    ret = at_tok_nextint(p_cur, &result);
    if (ret < 0)
    	return -1;
    if (!(result == 0 || result == 1))
    	return -1;
    if (p_out != NULL)
    	*p_out = (char)result;
    return ret;
}

int at_tok_nextstr(char **p_cur, char **p_out)
{
    if (*p_cur == NULL)
    	return -1;
    *p_out = nextTok(p_cur);
    return 0;
}

/** returns 1 on "has more tokens" and 0 if no */
int at_tok_hasmore(char **p_cur)
{
    return ! (*p_cur == NULL || **p_cur == '\0');
}
