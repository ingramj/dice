#include "evaluate.h"
#include "random.h"
#include "scanner.h"
#include <stdio.h>
#include <unistd.h>

static void print_banner(void)
{
	fprintf(stderr, "Dice Roller v0.1.1\n"
	        "Copyright (c) 2013 Jim Ingram <ingramj@gmail.com>\n"
	        "ctrl-d to quit\n");
}

int
main(void)
{
	print_banner();
	seed_random();
	fprintf(stderr, "> ");
	while (evaluate() >= 0) {
		fprintf(stderr, "> ");
	}

	fprintf(stderr, "\n");
	cleanup_evaluator();
	return 0;
}
