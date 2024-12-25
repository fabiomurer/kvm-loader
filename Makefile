SRC := $(shell ls -1 src/*.c)
OBJS := $(SRC:c=o)
CFLAGS := -Iincludes/

lvm: $(OBJS)
	gcc -o lvm $(OBJS)

%.o: %.c
	gcc ${CFLAGS} -o $@ -c $<

testprog:
	gcc -fno-pie -no-pie testprog/testprog.c -o tprog -nostdlib
	gcc -fno-pie -no-pie testprog/testprog_syscall.c -o tprog_syscall -nostdlib

clean:
	rm -rf ${OBJS} lvm

clangd-config:
	bear -- make

ldtest: lvm
	./lvm /usr/bin/ld.so

.PHONY: lvm testprog clean