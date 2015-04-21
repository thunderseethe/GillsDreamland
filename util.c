unsigned int char4ToInt(unsigned char* buf){
	unsigned int result = 0;
	for(int i = 0; i < 4; i++){
		result += buf[i] * (1 << i*8);
	}
	return result;
}

void intToChar4(unsigned int data, unsigned char* buf){
	for(int i = 0; i < 4; i++){
		buf[i] = (data >> (i)*8) & 0xFF;
	}
}