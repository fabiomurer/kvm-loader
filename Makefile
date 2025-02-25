SRC := $(shell ls -1 src/*.c)
LIBS := $(shell ls -1 includes/*.h)
OBJS := $(SRC:c=o)
CFLAGS := -O0 -g -Wall -Wextra -Iincludes/

lvm: $(OBJS)
	gcc -o lvm $(OBJS)

%.o: %.c %.h
	gcc ${CFLAGS} -o $@ -c $<

testprog: testprog/testprog.c testprog/testprog_syscall.c
	gcc -g -static testprog/testprog.c -o stprog
	gcc -g -static -nostdlib testprog/testprog_syscall.c -o ssyscall

clean:
	rm -rf ${OBJS} lvm stprog ssyscall

clangd-config:
	bear -- make

ldtest: lvm
	./lvm /usr/bin/ld.so

.PHONY: lvm testprog clean