#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>

static int read_int(char first);


int
get_token(void)
{
	int peek;
	int tok = END;

	do {
		peek = getc(stdin);
	} while (peek == ' ' || peek == '\t');

	switch (peek) {
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		tok = read_int(peek);
		break;
	case 'd': case 'D':
		tok = DICE_OP;
		break;
	case 'k': case 'K':
		tok = KEEP_OP;
		break;
	case '+':
		tok = ADD_OP;
		break;
	case '-':
		tok = SUB_OP;
		break;
	case '(':
		tok = LPAREN;
		break;
	case ')':
		tok = RPAREN;
		break;
	case EOF: case '\n':
		tok = END;
		break;
	default:
		fprintf(stderr, "unrecognized character\n");
	}

	return tok;
}


static int
read_int(char first)
{
	char buf[11]; /* INT_MAX is 10 digits. More than that and someone
	               * is fucking with us.*/
	int peek, i;

	buf[0] = first;
	buf[10] = '\0';
	for (i = 1; i < 10; i++) {
		peek = getc(stdin);
		switch (peek) {
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			buf[i] = peek;
			break;
		default:
			buf[i] = '\0';
			ungetc(peek, stdin);
			break;
		}
		if (buf[i] == '\0') break;
	}

	return atoi(buf);
}
