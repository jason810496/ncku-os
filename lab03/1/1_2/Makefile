judge:
	@gcc -o 1.out 1_2.c -lpthread
	@i=1; while [ $$i -le 100 ]; do \
		./1.out; \
		i=$$((i + 1)); \
	done
	@./judge.out
	@rm -f 1.out
	@rm -f 1.txt

my-judge:
	@gcc -o 1.out 1_2.c -lpthread
	@if [ -f 1.txt ]; then \
		rm -f 1.txt; \
	fi
	@i=1; while [ $$i -le 100 ]; do \
		./1.out; \
		i=$$((i + 1)); \
	done
	@if diff 1_ans.txt 1.txt > /dev/null; then \
		echo "Success"; \
	else \
		echo "Failed"; \
	fi
	@rm -f 1.out
	@rm -f 1.txt

diff:
	@gcc -o 1.out 1_2.c -lpthread
	@i=1; while [ $$i -le 100 ]; do \
		./1.out; \
		i=$$((i + 1)); \
	done
	@git diff --word-diff  1_ans.txt 1.txt || true
	@rm -f 1.out
	@rm -f 1.txt
