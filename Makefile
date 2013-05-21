CC      := clang
CFLAGS  := -std=c89 -Wall -Wextra -pedantic -D_XOPEN_SOURCE=500 -g
LDFLAGS := -g

TARGET = dice
OBJS   = main.o evaluate.o random.o scanner.o
DEPS   = $(OBJS:.o=.d)

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -MMD -c $< -o $@

-include $(DEPS)

.PHONY: clean cleaner
clean:
	rm -f $(OBJS) $(DEPS)

cleaner: clean
	rm -f $(TARGET)
