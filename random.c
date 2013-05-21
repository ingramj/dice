#include "random.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>


void
seed_random(void)
{
	/* Single iteration of Xorshift to get a good seed. */
	unsigned int seed = time(NULL) ^ getpid();
	seed ^= (seed << 19);
	seed ^= (seed >> 11);
	seed ^= (seed << 9);

	fprintf(stderr, "seed = %u\n", seed);
	srandom(seed);
}


int
random_range(int min, int max)
{
	int range, bucket, remainder, r;

	range = max - min;
	bucket = RAND_MAX / range;
	remainder = RAND_MAX % range;

	while ((r = random()) > (RAND_MAX - remainder))
		;

	return min + (r / bucket);
}
