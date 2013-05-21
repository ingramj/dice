#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>

static int read_int(char first);


int
get_token(void)
{
	int peek;
	int tok = ENDINPUT;

	do {
		peek = getc(stdin);
	} while (peek == ' ' || peek == '\t');

	switch (peek) {
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		tok = read_int(peek);
		break;
	case '%':
		tok = 100;
		break;
	case 'd': case 'D':
		tok = DICE_OP;
		break;
	case 'k': case 'K':
		tok = KEEP_OP;
		break;
	case '*':
		tok = MUL_OP;
		break;
	case '/':
		tok = DIV_OP;
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
	case '\n':
		tok = ENDLINE;
		break;
	case EOF:
		tok = ENDINPUT;
		break;
	default:
		fprintf(stderr, "unrecognized character\n");
	}

	return tok;
}


/* isdigit() depends on locale, but we only want ASCII digits. */
static int
digit(int c)
{
	return (c == '0' || c == '1' || c == '2' || c == '3' ||
	        c == '4' || c == '5' || c == '6' || c == '7' ||
	        c == '8' || c == '9');
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
		if (digit(peek)) {
			buf[i] = peek;
		} else {
			buf[i] = '\0';
			break;
		}
	}

	if (i == 10) {
		peek = getc(stdin);
		if (digit(peek)) {
			fprintf(stderr, "integer constant has too many digits\n");
			return ENDINPUT;
		}
	}
	ungetc(peek, stdin);

	return atoi(buf);
}
