SRC := $(shell ls -1 *.c)
OBJS := $(SRC:c=o)

all: $(OBJS)
	gcc -o lvm $(OBJS)
%.o: %.c
	gcc -o $@ -c $<
testprog:
	gcc -fno-pie -m32 -no-pie testprog/testprog.c -o tprog -nostdlib
clean:
	rm -rf *.o lvm

.PHONY: all testprog clean
