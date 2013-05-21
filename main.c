#include "evaluate.h"
#include "random.h"
#include "scanner.h"
#include <stdio.h>

int
main(void)
{
	printf("Dice Roller v0.1\n");
	printf("ctrl-d to quit\n");
	seed_random();
	printf("> ");
	while (evaluate() >= 0) {
		printf("> ");
	}

	printf("\n");
	cleanup_evaluator();
	return 0;
}
