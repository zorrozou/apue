all: tbfwrite.c tbfwrite.h test.c

test: test.c tbfwrite.o
	gcc -o $@ $^
%.o: %.c %.h
	gcc -o $@ -c $<

