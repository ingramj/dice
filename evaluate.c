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
static int copyn_seq(seq_t *dest, seq_t *src, int num);
static int compare_int_descending(const void *a, const void *b);

#define SEQ_STACK_SIZE 1024
static int seq_stack_p = 0;
static seq_t seq_stack[SEQ_STACK_SIZE];
static void pop_seq_stack(void);
static void clean_seq_stack(void);


/* By comparison, the operator stack is simple. */
#define OP_STACK_SIZE 512
static int op_stack_p = 0;
static int op_stack[OP_STACK_SIZE];

static int eval_op(int op);

/* An operator's precedence has index = -token - 1. */
static int precedence[] = { 10, 9, 8, 8, 7, 7, 0, 0 };


/* Error reporting/recovery macro */
#define check(cond, msg) \
	do { \
	if (!(cond)) { \
		fprintf(stderr, "%s\n", msg); \
		goto error; \
	}} while (0)

int
evaluate(void)
{
	/* Shunting Yard Algorithm. */
	int tok;

	/* Get a token. */
	tok = get_token();
	while (tok != ENDLINE && tok != ENDINPUT) {
		if (tok >= 0) {
			/* If it is a number, add it to the sequence stack. */
			check(seq_stack_p < SEQ_STACK_SIZE, "sequence stack overflow");
			addto_seq(&seq_stack[seq_stack_p], tok);
			seq_stack_p++;
		} else if (tok >= SUB_OP) {
			/* If it is an operator, then while there are operators on the
			 * stack... */
			while (op_stack_p > 0) {
				int cur_prec = precedence[-tok - 1];
				int top_op = op_stack[op_stack_p - 1];
				int top_prec = precedence[-top_op - 1];
				/* of greater or equal precedence... */
				if (top_prec < cur_prec) {
					break;
				}
				/* pop off the other operators and evaluate them, */
				op_stack_p--;
				if (eval_op(top_op) < 0) {
					goto error;
				}
			}
			/* Push the original operator. */
			check(op_stack_p < OP_STACK_SIZE, "operator stack overflow");
			op_stack[op_stack_p++] = tok;
		} else if (tok == LPAREN) {
			/* If it is a left parenthesis, push it onto the stack. */
			check(op_stack_p < OP_STACK_SIZE, "operator stack overflow");
			op_stack[op_stack_p++] = tok;
		} else if (tok == RPAREN) {
			/* If it is a right parenthesis, then pop operators and
			 * evaluate them until a match is found. */
			int matched = 0;
			int op_count = 0;
			while (op_stack_p > 0) {
				int top_op = op_stack[op_stack_p - 1];
				if (top_op == LPAREN) {
					matched = 1;
					op_stack_p--;
					break;
				}
				op_count++;
				op_stack_p--;
				if (eval_op(top_op) < 0) {
					goto error;
				}
			}
			check(op_count > 0, "missing operator");
			check(matched, "mismatched parentheses");
		}
		tok = get_token();
	}

	/* If there are no more tokens, then pop the remaining
	 * operators and evaluate them. */
	while (op_stack_p > 0) {
		int top_op = op_stack[op_stack_p - 1];
		check(top_op != LPAREN && top_op != RPAREN, "mismatched parentheses");
		op_stack_p--;
		if (eval_op(top_op) < 0) {
			goto error;
		}
	}

	if (seq_stack_p > 1) {
		fprintf(stderr, "%d values left on the sequence stack; should be 1\n",
			seq_stack_p);
		/* this means the evaluation went wrong, and we should exit */
		return -1;
	} else if (seq_stack_p == 0) {
		return -1;
	}

	printf("%d\n", seq_stack[0].sum);
	pop_seq_stack();

	if (tok == ENDLINE) {
		return 0;
	}
	return -1;

error:
	/* Reset the interpreter to a known state. */
	cleanup_evaluator();
	return 1;
}


void
cleanup_evaluator(void)
{
	clean_seq_stack();
	op_stack_p = 0;
}


static void
addto_seq(seq_t *s, int val)
{
	assert(s != NULL);

	if (s->cap == 0) {
		s->vals = calloc(8, sizeof(int));
		assert(s->vals != NULL);
		s->cap = 8;
	} else if (s->size >= s->cap) {
		int *new_vals = realloc(s->vals, (s->cap*2) * sizeof(int));
		assert(new_vals != NULL);
		s->vals = new_vals;
		s->cap *= 2;
	}
	s->vals[s->size++] = val;
	s->sum += val;
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
	seq_stack_p--;
	seq_stack[seq_stack_p].sum = 0;
	seq_stack[seq_stack_p].size = 0;
}


static void
clean_seq_stack(void)
{
	int i;

	for (i = 0; i < SEQ_STACK_SIZE; i++) {
		if (seq_stack[i].cap > 0) {
			seq_stack[i].cap = 0;
			free(seq_stack[i].vals);
		}
		seq_stack[i].size = seq_stack[i].sum = 0;
	}
	seq_stack_p = 0;
}


/* Helper-macro for operator evaluation. The argument should be some C
 * operator (+, -, *, or /). */
#define DO_ARITH(op) \
	do { \
		n2 = seq_stack[seq_stack_p - 1].sum; \
		pop_seq_stack(); \
		n1 = seq_stack[seq_stack_p - 1].sum; \
		pop_seq_stack(); \
		addto_seq(&seq_stack[seq_stack_p], n1 op n2); \
		seq_stack_p++; \
	} while (0)


static int
eval_op(int op)
{
	int i, n1, n2;
	seq_t *s1, s2;

	s2.sum = s2.cap = s2.size = 0;
	s2.vals = NULL;

	/* All our operators are binary */
	if (seq_stack_p < 2) {
		fprintf(stderr, "not enough operands\n");
		return -1;
	}
	switch (op) {
	case DICE_OP: /* (number number -- sequence) */
		n2 = seq_stack[seq_stack_p - 1].sum;
		pop_seq_stack();
		n1 = seq_stack[seq_stack_p - 1].sum;
		pop_seq_stack();
		for (i = 0; i < n1; i++) {
			addto_seq(&seq_stack[seq_stack_p], random_range(1, n2+1));
		}
		seq_stack_p++;
		break;
	case KEEP_OP: /* (sequence number -- sequence) */
		n1 = seq_stack[seq_stack_p - 1].sum;
		pop_seq_stack();
		s1 = &seq_stack[seq_stack_p - 1];
		if (copyn_seq(&s2, s1, n1) < 0) {
			return -1;
		}
		pop_seq_stack();
		for (i = 0; i < n1; i++) {
			addto_seq(&seq_stack[seq_stack_p], s2.vals[i]);
		}
		seq_stack_p++;
		free(s2.vals);
		break;
	case MUL_OP:  /* (number number -- number) */
		DO_ARITH(*);
		break;
	case DIV_OP:  /* (number number -- number) */
		DO_ARITH(/);
		break;
	case ADD_OP:  /* (number number -- number) */
		DO_ARITH(+);
		break;
	case SUB_OP:  /* (number number -- number) */
		DO_ARITH(-);
		break;
	default:
		fprintf(stderr, "unrecognized operation\n");
		return -1;
	}
	return 0;
}
