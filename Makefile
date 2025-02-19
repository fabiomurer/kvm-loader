SRC := $(shell ls -1 src/*.c)
OBJS := $(SRC:c=o)
CFLAGS := -O0 -g -Wall -Wextra -Iincludes/

lvm: $(OBJS)
	gcc -o lvm $(OBJS)

%.o: %.c
	gcc ${CFLAGS} -o $@ -c $<

testprog: testprog/testprog.c testprog/testprog_syscall.c
	gcc -static testprog/testprog.c -o tprog_static_pie
	gcc -fno-pie -no-pie -static testprog/testprog.c -o tprog_static
	gcc testprog/testprog.c -o tprog_dynamic
	gcc -fno-pie -no-pie testprog/testprog.c -o tprog_dynamic_pie
	gcc -fno-pie -no-pie testprog/testprog_syscall.c -o tprog_syscall -nostdlib

clean:
	rm -rf ${OBJS} lvm

clangd-config:
	bear -- make

ldtest: lvm
	./lvm /usr/bin/ld.so

.PHONY: lvm testprog clean