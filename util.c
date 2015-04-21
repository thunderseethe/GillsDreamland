unsigned int char4ToInt(char* buf){
	unsigned int result = 0;
	for(int i = 0; i < 4; i++){
		result += buf[i] * (1 << i*8);
	}
	return result;
}