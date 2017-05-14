#include "lex.h"
#include "ucl.h"
#include "keyword.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "config.h"

#define CURSOR		(Input.cursor)
#define LINE		(Input.line)
#define LINEHEAD	(Input.lineHead)

union value TokenValue;

union value TokenValue;
struct coord TokenCoord;
struct coord PrevCoord;




static void Skipwhilespace()
{
	int ch = *CURSOR;
	while (ch == '\n' || ch == '\t' || ch == '\v' || ch == '\f' ||
			ch == ' ' || ch == '\r' || ch == '/' || ch == '#')
	{
		switch (ch)
		{
		case '\n':
			TokenCoord.ppline++;
			LINE++;
			LINEHEAD = ++CURSOR;
			break;
		case '#':
			//to do
			break;
		case '/':
			if (CURSOR[1] != '/' && CURSOR[1] != '*')
				return;
			CURSOR++;
			if (*CURSOR == '/') {
				CURSOR++;
				while (CURSOR != '\n'&& CURSOR != END_OF_FILE)//bug
				{
					CURSOR++;
				}
			}
			else
			{
				while (CURSOR != '*'&& CURSOR[1] != '/')
				{
					if (*CURSOR == '\n')
					{
						TokenCoord.ppline++;
						LINE++;
					}
					//this is bug
					CURSOR++;
				}
				CURSOR += 2;
			}
			break;

		default:
			CURSOR++;
			break;
		}
	}
}


static int ScanIntLiteral(unsigned char *start, int len, int base)
{
	unsigned char *p = start;
	unsigned char *end = start + len;
	unsigned int i[2] = { 0, 0 };
	int tok = TK_INTCONST;
	int d = 0;
	int carry0 = 0, carry1 = 0;
	int overflow = 0;

	while (p != end)
	{
		if (base == 16)
		{
			if ((*p >= 'A' && *p <= 'F') ||
				(*p >= 'a' && *p <= 'f'))
			{
				d = ToUpper(*p) - 'A' + 10;
			}
			else
			{
				d = *p - '0';
			}
		}
		else
		{
			d = *p - '0';
		}

		switch (base)
		{
			case 16:
				carry0 = HIGH_4BT(i[0]);
				carry1 = HIGH_4BT(i[1]);
				i[0] = i[0] << 4;
				i[1] = i[1] << 4;
				break;

			case 8:
				carry0 = HIGT_3BT(i[0]);
				carry1 = HIGT_3BT(i[1]);
				i[0] = i[0] << 3;
				i[1] = i[1] << 3;
				break;

			case 10:
			{

				unsigned int t1, t2;
				carry0 = HIGH_3BIT(i[0]) + HIGH_1BIT(i[0]);
				carry1 = HIGT_3BIT(i[1]) + HIGH_1BIT(i[1]);
				t1 = i[0] << 3;
				t2 = i[0] << 1;
				if (t1 > UNIT_MAX - t2)
				{
					carry0++;
				}
				i[0] = t1 + t2;
				t1 = i[1] << 3;
				t2 = i[1] << 1;
				if (t1 > UNIT_MAX - t2)
				{
					carry1++;
				}
				i[1] = t1 + t2;
			}
			break;
		}
		if (i[0] > UINT_MAX - d)
		{
			carry0 += i[0] - (UNIT_MAX - d);
		}
		if (carry1 || (i[1] > UNIT_MAX - carry0))
		{
			overflow = 1;
		}
		i[0] += d;
		i[1] += carry0;
		p++;
	}
	/*****************************
	overflow != 0:
	out of 64 bit bound
	i[1] != 0
	out of 32 bit bound
	*****************************/
	if (overflow || i[1] != 0)
	{
		Warning(&TokenCoord, "Integer literal is too big");
	}

	TokenValue.i[1] = 0;
	TokenValue.i[0] = i[0];
	tok = TK_INTCONST;
	/////////// 12345678U

	if (*CURSOR == 'U' || *CURSOR == 'u')
	{
		CURSOR++;
		if (tok == TK_INTCONST)
		{
			tok = TK_UINTCONST;
		}
		else if (tok == TK_LLONGCONST)
		{
			tok = TK_ULLONGCONST;
		}
	}
	// 12345678UL

	if (*CURSOR == 'L' || *CURSOR == 'l')
	{
		CURSOR++;
		if (tok == TOK_INTCONST)
		{
			tok = TK_LONGCONST;
		}
		else if (tok == TK_UINTCONST)
		{
			tok = TK_ULONGCONST;
		}
		if (*CURSOR == 'L' || *CURSOR == 'l')
		{
			CURSOR++;
			if (tok < TK_LLONGCONST)
			{
				tok = TK_LLONGCONST;
			}
		}
	}
	return tok;
}

static int ScanfCharLiteral(void)
{
	int ch = 0;
	int count = 0;
	int wide = 0;

	if (*CURSOR == 'L' || *CURSOR == 'l')
	{
		wide = 1;
		*CURSOR++;
	}
	while (*CURSOR != '\'')
	{
		if (*CURSOR == '\n' || *CURSOR == END_OF_FILE)
			break;
		ch = *CURSOR == '\\' ? ScanEscapeChar(wide) : *CURSOR++;
		count++;
	}

	if (*CURSOR != '\'')
	{
		Error(&TokenCoord, "Expect'");
		goto end_char;
	}

	*CURSOR++;
	if (count > 1)
	{
		Warning(&TokenCoord, "Tow many characters");
	}

end_char:
	TokenValue.i[0] = ch;
	TokenValue.i[0] = 0;

	return TK_INTCONST;
}


static int ScanStringLiteral(void) //"abc" or L"abc"
{
	char tmp[512];
	int maxlen = 512;
	int len=0;
	char * cp;
	int ch;
	int wide=0;
	String str;
	CALLOC(str);
	if (*CURSOR == 'L' || *CURSOR == 'l')
	{
		//todo;
	}

next_string:
	while (*CURSOR != '"')
	{
		if (*CURSOR == '\n' || *CURSOR == END_OF_FILE)
			break;

		ch = *CURSOR == '\\' ? ScanEscapeChar(wide) : *CURSOR++;
		if (wide)
		{
			//todo
		}
		else
		{
			cp[len] = (char)ch;
		}
		len++;
		if (len >= maxlen)
		{
			AppendSTR(str, tmp, len, wide);
			len = 0;
		}
	}

	if (*CURSOR != '"')
	{
		Error(&TokenCoord, "Expect\"");
		goto end_string;
	}

	CURSOR++;
	SkipWhiteSpace();
	if (CURSOR[0] == '"')
	{
		if (wide == 1)
		{
			Error(&TokenCoord, "String wideness mismatch");
		}
		CURSOR++;
		goto next_string;
	}

end_string:
	AppendSTR(str, tmp, len, wide);
	TokenValue.p = str;

	return wide ? TK_WIDESTRING : TK_STRING;
}




static int ScanEscapeChar(int wide)
{
	int c = 0, of = 0;

	CURSOR++;
	switch (*CURSOR++)
	{
	case '\'':
	case '\"':
	case'\?':
	case '\\':
		return *(CURSOR - 1);
	case 'a':
		return '\a';
	case 'b':
		return '\b';
	case 'v':
		return '\v';
	case 'f':
		return '\f';
	case 't':
		return '\t';
	case 'n':
		return '\n';
	case 'r':
		return '\r';
	case 'x':
		//to do
		break;
	default:
		//todo
		return -1;
	}
}

static int FindKeyWord(char *str, int len)
{
	struct keyword *p = NULL;
	//index is 0 when __int64,see keyword.h

	int index = 0;
	if (*str != '_')
		index = ToUpper(*str) - 'A' + 1;

	p = keywords[index];
	while (p->name)
	{
		if (p->len == len && strncmp(str, p->name, len) == 0)
		{
			break;
		}
		p++;
	}
	return p->tok;

}

static int ScanNumbericLiteral(void)
{
	unsigned char *start = CURSOR;
	int base = 10;

	if (*CURSOR == '.')
	{
		//todo;
	}
	if (*CURSOR == '0' && (CURSOR[1] == 'x' || CURSOR[1] == 'X'))
	{
		CURSOR += 2;
		start = CURSOR;
		base = 16;
		if (!IsHexDigit(*CURSOR))//0x223
		{
			Error(&TokenCoord, "Expect hex digit");
			TokenValue.i[0] = 0;
			return TK_INTCONST;
		}
		while (IsHexDigit(*CURSOR)) {
			CURSOR++;
		}
	}
	else if (*CURSOR == '0')//021312
	{
		CURSOR++;
		base = 8;
		while (IsOctDigit(*CURSOR))
		{
			CURSOR++;
		}
	}
	else
	{
		CURSOR++;
		while (IsDigit(*CURSOR))
		{
			CURSOR++;
		}
	}

	if (base == 16 || (*CURSOR != '.' && *CURSOR != 'e' && *CURSOR != 'E'))
	{
		return ScanIntLiteral(start, (int)(CURSOR - start), base);
	}
	else
	{
		return ScanFloatLiteral(start);
	}

}




int GetNextToken(void)
{
	int tok;

	PrevCoord = TokenCoord;
	Skipwhilespace();
	TokenCoord.line = LINE;
	TokenCoord.col = (int) (CURSOR - LINEHEAD + 1);

	tok = (*Scanners[*CURSOR])();
	return tok;
}

void PeekToken(void)
{
	PeekPoint = CURSOR;
	PeekValue = TokenValue;
	PeekCoord = TokenCoord;
}
