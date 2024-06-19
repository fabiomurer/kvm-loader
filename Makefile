SRC := $(shell ls -1 src/*.c)
OBJS := $(SRC:c=o)
CFLAGS := -Iincludes/

all: $(OBJS)
	gcc -o lvm $(OBJS)
%.o: %.c
	gcc ${CFLAGS} -o $@ -c $<
testprog:
	gcc -fno-pie -m32 -no-pie testprog/testprog.c -o tprog -nostdlib
clean:
	rm -rf ${OBJS} lvm

.PHONY: all testprog clean
