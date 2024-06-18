__attribute__((naked)) int _start(void)
{
	asm("out %0, %1" : : "a" ('X'), "Nd" (0x3f8) : );
	asm("hlt");
	return 0;
}
