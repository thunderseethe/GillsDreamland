//Converts a buffer of char[4] to an integer
unsigned int char4ToInt(unsigned char* buf){
	unsigned int result = 0;
	for(int i = 0; i < 4; i++){
		//Increase result by buf[i] shifted up by i bytes
		result += buf[i] * (1 << i*8);
	}
	return result;
}

//Converts an integer to a buffer of char[4]
void intToChar4(unsigned int data, unsigned char* buf){
	for(int i = 0; i < 4; i++){
		//Set buf[i] to byte i of integer (data / 2^i*8) & 0xFF
		buf[i] = (data >> i*8) & 0xFF;
	}
}