#include "lex.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void Skipwhilespace()
{
	

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
