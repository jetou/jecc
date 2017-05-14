struct keyword
{
	char *name;
	int len;
	int tok;
};

static struct keyword keywords_[] = 
{
	{"__int64", 0, TK_INT64},
	{NULL,		0,	TK_ID}
};

static struct keyword keywordsA[] =
{
	{"auto", 4, TK_AUTO},
	{NULL,   0, TK_ID}

};

static struct keyword keywordsB[]=
{
	{"break", 5, TK_BREAK},
	{NULL,	  0, TK_ID}	
};

static struct keyword keywordsC[]=
{
	{"const", 5, TK_CONST},
	{"continue", 8, TK_CONTINUE},
	{"}
}


static struct keyword *keywords[] =
{
	keywords_, keywordsA, keywordsB, keywordsC,
	keywordsD, keywordsE, keywordsF, keywordsG,
	keywordsH, keywordsI, keywordsJ, keywordsK,
	keywordsL, keywordsM, keywordsN, keywordsO,
	keywordsP, keywordsQ, keywordsR, keywordsS,
	keywordsT, keywordsU, keywordsV, keywordsW,
	keywordsX, keywordsY, keywordsZ
};
