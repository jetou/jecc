#ifndef LEX_H
#define LEX_H

#define MAX_LEN;

typedef union{
	char name[MAX_LEN];
	int valuelen;
}Value;

typedef enum {
	#define TOKEN(kind, name) Kind,
	#include "token.txt"
	#undef TOKEN
}TokenKind;


typedef struct {
	TokenKind Kind;
	Value value;
}Token_j;

typedef struct lexer_t {
	token_j *untoken;

	FILE *fp;
	const char *fname;
	unsigned int line;
	unsigned int column;
	unsigned int prev_colum;
}lexer_j;

int GetNextToken(void);
void PeekToken(void);
#endif
