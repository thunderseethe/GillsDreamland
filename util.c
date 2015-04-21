#include "util.h"

unsigned int char4ToInt(unsigned char* buf){
	unsigned int result = 0;
	for(int i = 0; i < 4; i++){
		printf("buf[%d] = %d * %d = %d\n", i, buf[i], (1 << i*8), buf[i] * (1 << i*8));
		result += buf[i] * (1 << i*8);
	}
	printf("%d\n", result);
	return result;
}

void intToChar4(unsigned int data, unsigned char* buf){
	for(int i = 0; i < 4; i++){
		buf[i] = (data >> i*8) & 0xFF;
	}
}