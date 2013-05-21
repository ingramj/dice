#include "evaluate.h"
#include "random.h"
#include "scanner.h"
#include <stdio.h>

int
main(void)
{
	int tok;
	printf("> ");
	while ((tok = get_token()) != ENDINPUT) {
		if (tok >= 0) {
			printf("INTEGER: %d\n", tok);
		} else if (tok != ENDLINE) {
			printf("TOKEN:   %d\n", tok);
		} else {
			printf("> ");
		}
	}
	return 0;
}
