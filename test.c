#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
	unsigned char* buf = malloc(sizeof(unsigned char)*4);
	buf[0] = 0x39;
	buf[1] = 0x30;
	buf[2] = 0x00;
	buf[3] = 0x00;
	unsigned int result = 0;
	for(int i = 0; i < 4; i++){
		printf("%d * %d = %d\n", buf[i], (1 << i*8), buf[i] * (1 << i*8));
    	result += buf[i] * (1 << i*8);
    }
	printf("%d\n",result);
	return 0;
}