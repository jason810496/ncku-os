CC = gcc
CFLAGS = -pthread
SRC = 1_1.c
OUT = 1.out
OUTPUT = 1.txt
ANSWER = 1_ans.txt

.DEFAULT_GOAL := my-judge

judge:
	@$(CC) $(CFLAGS) $(SRC) -o $(OUT)
	@i=1; while [ $$i -le 100 ]; do \
		./1.out; \
		i=$$((i + 1)); \
	done
	@./judge.out
	@rm -f 1.out
	@rm -f 1.txt

my-judge:
# remove 1.txt if it exists
# if check the difference between 1_ans.txt and 1.txt, if there is no difference, then print Success, otherwise print Failed
	@$(CC) $(CFLAGS) $(SRC) -o $(OUT)
	@if [ -f $(OUTPUT) ]; then \
		rm -f $(OUTPUT); \
	fi
	@i=1; while [ $$i -le 100 ]; do \
		./$(OUT); \
		i=$$((i + 1)); \
	done
	@if diff $(ANSWER) $(OUTPUT) > /dev/null; then \
		echo "Success"; \
	else \
		echo "Failed"; \
	fi
	@rm -f $(OUT)
	@rm -f $(OUTPUT)

diff:
	@$(CC) $(CFLAGS) $(SRC) -o $(OUT)
	@i=1; while [ $$i -le 100 ]; do \
		./$(OUT); \
		i=$$((i + 1)); \
	done
	@git diff --word-diff $(ANSWER) $(OUTPUT) || true
	@rm -f $(OUT)
	@rm -f $(OUTPUT)
