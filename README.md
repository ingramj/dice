Dice: An interactive dice roller
================================

Dice reads in expressions in a subset of [dice notation][] from stdin,
evaluates them, and prints the result to stdout. It works like a
regular calculator supporting addition, subtraction, and parentheses,
along with two additional operators: `d` (for "dice"), and `k` (for
"keep").

The "dice" operator requires two operands, which may be arbitrarily
complex expressions. Omitting an operand is an error.

    1d6         # Roll 1 six-sided die.
    (2+1)d6     # Roll 2+1=3 six-sided dice.
    2+1d6       # Add 2 to 1d6.
    (1d4)d(3d6) # Roll 1d4 dice with 3d6 sides each.

The `d` operator has a higher precedence than `+`. The last example is
admittedly silly. Note that each subexpression is only evaluated once.

The "keep" operator requires two operands, and keeps a number (its
right operand) of dice with the highest value from its left operand.
The left operand should be a dice expression:

    4d6k3       # this works
    4d6k3 + 1   # so does this
    (4d6+1)k3   # this does not

The `k` operator has a higher precedence than addition or subtraction,
but lower than `d`.

If you wish to use dice non-interactively, you can pipe an expression
into it. If you do that, you'll probably want to redirect stderr to
/dev/null, to avoid extra output:

    $ echo "2d6+1d4+2" | ./dice 2> /dev/null


I wrote this over the weekend, so there are probably bugs. Especially
in the evaluator; it's a bit of a mess.

[dice notation]: http://en.wikipedia.org/wiki/Dice_notation