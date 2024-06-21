char C = 'O';

void print(char *s)
{
	while(*s) {
		asm("out %0, %1" : : "a" (*s), "Nd" (0x3f8) : );
		s++;
	}
}

int _start(void)
{
	print("Hello!\n");
	asm("hlt");
	return 0;
}
