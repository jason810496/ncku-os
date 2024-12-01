CC = gcc
CFLAGS = -pthread
SRC1 = 2_1.c
SRC2 = 2_2.c
OUT = 2.out
OUTPUT = 2.txt
ANSWER1 = 2_1_ans.txt
ANSWER2 = 2_2_ans.txt

my-judge1:
	@$(CC) $(CFLAGS) $(SRC1) -o $(OUT)
	@rm -f $(OUTPUT)
	@./$(OUT)
	@if diff $(ANSWER1) $(OUTPUT) > /dev/null; then \
		echo "Success"; \
	else \
		echo "Failed"; \
	fi
	@rm -f $(OUT)
	@rm -f $(OUTPUT)

my-judge2:
	@$(CC) $(CFLAGS) $(SRC2) -o $(OUT)
	@rm -f $(OUTPUT)
	@i=1; while [ $$i -le 10 ]; do \
		./$(OUT); \
		i=$$((i + 1)); \
	done
	@if diff $(ANSWER2) $(OUTPUT) > /dev/null; then \
		echo "Success"; \
	else \
		echo "Failed"; \
	fi
	@rm -f $(OUT)
	@rm -f $(OUTPUT)

judge1:
	$(CC) $(CFLAGS) $(SRC1) -o $(OUT)
	@./2.out
	@./judge.out 1
	@rm -f 2.out
	@rm -f 2.txt

judge2:
	$(CC) $(CFLAGS) $(SRC2) -o $(OUT)
	@i=1; while [ $$i -le 10 ]; do \
		./2.out; \
		i=$$((i + 1)); \
	done
	@./judge.out 2
	@rm -f 2.out
	@rm -f 2.txt

diff1:
	@$(CC) $(CFLAGS) $(SRC1) -o $(OUT)
	@rm -f $(OUTPUT)
	@./$(OUT)
	@git diff --word-diff $(ANSWER1) $(OUTPUT) || true
	@rm -f $(OUT)
	@rm -f $(OUTPUT)

diff2:
	@$(CC) $(CFLAGS) $(SRC2) -o $(OUT)
	@rm -f $(OUTPUT)
	@i=1; while [ $$i -le 10 ]; do \
		./$(OUT); \
		i=$$((i + 1)); \
	done
	@git diff --word-diff $(ANSWER2) $(OUTPUT) || true
	@rm -f $(OUT)
	@rm -f $(OUTPUT)

clean:
	if [ -f 2.out ]; then \
		rm -f 2.out; \
	fi