#include "evaluate.h"
#include "scanner.h"
#include "random.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


/* Expressions actually evaluate to sequences of numbers. This is
 * because the 'keep' operator needs access to the individual dice
 * values, not just their total. We need a stack to keep track of
 * sequences.
 */
typedef struct seq {
	int sum;
	int cap;
	int size;
	int *vals;
} seq_t;

static void addto_seq(seq_t *s, int val);
static int copy_seq(seq_t *dest, seq_t *src, int num);
static int compare_int_descending(const void *a, const void *b);

#define SEQ_STACK_SIZE 1024
static int seq_stack_p;
static seq_t seq_stack[SEQ_STACK_SIZE];
static void pop_seq_stack(void);
static void clean_seq_stack(void);


/* By comparison, the operator stack is simple. */
#define OP_STACK_SIZE 512
static int op_stack_p;
static int op_stack[OP_STACK_SIZE];


/* An operator's precedence has index = -token - 1. */
static int precedence[] = { 10, 9, 8, 8, 0, 0 };


int
evaluate(void)
{
	/* TODO: stub */
	return 0;
}


static void
addto_seq(seq_t *s, int val)
{
	assert(s != NULL);

	if (s->cap == 0) {
		s->vals = calloc(8, sizeof(int));
		s->cap = 8;
	} else if (s->size >= s->cap) {
		s->vals = realloc(s->vals, s->cap * 2);
		s->cap *= 2;
	}
	assert(s->vals);

	s->vals[s->size++] = val;
}


static int
copyn_seq(seq_t *dest, seq_t *src, int num)
{
	int i;

	assert(dest != NULL);
	assert(src != NULL);

	if (num > src->size) {
		fprintf(stderr, "can't keep %d dice, there are only %d!\n",
		        num, src->size);
		return -1;
	}

	qsort(src->vals, src->size, sizeof(int), compare_int_descending);
	dest->size = 0;

	for (i = 0; i < num; i++) {
		addto_seq(dest, src->vals[i]);
	}

	return 0;
}


static int
compare_int_descending(const void *ap, const void *bp)
{
	int a = *(int *)ap;
	int b = *(int *)bp;
	if (a < b) {
		return 1;
	} else if (a > b) {
		return -1;
	}
	return 0;
}


static void
pop_seq_stack(void)
{
	seq_stack[seq_stack_p].sum = 0;
	seq_stack[seq_stack_p].size = 0;
	seq_stack_p--;
}


static void
clean_seq_stack(void)
{
	int i;

	for (i = 0; i < SEQ_STACK_SIZE; i++) {
		if (seq_stack[i].vals != NULL) {
			free(seq_stack[i].vals);
		}
	}
}
