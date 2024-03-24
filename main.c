// build-in LED: PC7

#define PORTC (*((volatile unsigned char *)0x48000BFF))

int main()
{
	PORTC = ~(PORTC & 0);
	return 0;
}
