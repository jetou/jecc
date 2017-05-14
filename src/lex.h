#ifndef LEX_H
#define LEX_H

#define MAX_LEN   150

union value{
	int i[2];
	float f;
	double d;
	void *p;
};

enum  token{
	TK_BEGIN,
	#define TOKEN(kind, name) Kind,
	#include "token.h"
	#undef TOKEN
};




#define IsDigit(c)		(c >= '0' && c <= '9')
#define IsOctDigit(c)	(c >= '0' && c <= '7')
#define IsHexDigit(c)	(isdigit(c) || (c >= 'A' && c <= 'F'))
#define ToUpper(c)		(c & ~0x20)




int GetNextToken(void);
void PeekToken(void);
#endif
