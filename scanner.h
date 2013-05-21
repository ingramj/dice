#ifndef SCANNER_H_
#define SCANNER_H_

/* Tokens: negative numbers indicate valueless tokens.
 *         positive numbers indicate integer tokens.
 */
#define DICE_OP -1
#define KEEP_OP -2
#define ADD_OP  -3
#define SUB_OP  -4
#define LPAREN  -5
#define RPAREN  -6
#define END     -7

int get_token(void);

#endif
