#include "encoding.h"

static void swap(unsigned char* buf, int i, int j) {
	char tmp = buf[i];
	buf[i] = buf[j];
	buf[j] = tmp;
}

void encode(unsigned char* buf) {
	static const unsigned char key[] = {0xF6, 0xE4, 0x76, 0x47, 0x54, 0xE6, 0x76};
	for (int i = 0; i < 7; i++) {
		buf[i + 2] += key[i];
	}
	buf[8] = (buf[2] >> 5) | (buf[8] << 3);
	for (int i = 2; i < 8; i++) {
		buf[i] = (buf[i + 1] >> 5) | (buf[i] << 3);
	}
	// TODO: disassembled code has a loop apparently XORing the buffer with zeroes, which appears doing nothing
	swap(buf, 2, 5);
	swap(buf, 4, 8);
	swap(buf, 6, 7);
}
