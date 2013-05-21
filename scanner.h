#ifndef SCANNER_H_
#define SCANNER_H_

/* Tokens: negative numbers indicate valueless tokens.
 *         positive numbers indicate integer tokens.
 */
#define DICE_OP  -1
#define KEEP_OP  -2
#define MUL_OP   -3
#define DIV_OP   -4
#define ADD_OP   -5
#define SUB_OP   -6
#define LPAREN   -7
#define RPAREN   -8
#define ENDLINE  -9
#define ENDINPUT -10

int get_token(void);

#endif
