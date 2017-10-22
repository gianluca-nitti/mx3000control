#include "encoding.h"

void encode(unsigned char* buf) {
	static const unsigned char key[] = {0xF6, 0xE4, 0x76, 0x47, 0x54, 0xE6, 0x76};
	for (int i = 0; i < 7; i++) {
		buf[i + 2] = (unsigned char) ((int)buf[i + 2] + (int)key[i]);
	}
	int bl, cl, dl;
	cl = buf[2];
	dl = cl;
	dl >>= 5;
	int esp10 = dl;
	dl = buf[3];
	bl = dl;
	bl >>= 5;
	cl <<= 3;
	bl |= cl;
	cl = buf[4];
	buf[2] = bl;
	bl = cl;
	bl >>= 5;
	dl <<= 3;
	bl |= dl;
	dl = buf[5];
	buf[3] = bl;
	bl = dl;
	bl >>= 5;
	cl <<= 3;
	bl |= cl;
	cl = buf[6];
	buf[4] = bl;
	bl = cl;
	bl >>= 5;
	dl <<= 3;
	bl |= dl;
	dl = buf[7];
	buf[5] = bl;
	bl = dl;
	bl >>= 5;
	cl <<= 3;
	bl |= cl;
	cl = buf[8];
	buf[6] = bl;
	bl = cl;
	bl >>= 5;
	dl <<= 3;
	bl |= dl;
	dl = esp10;
	cl <<= 3;
	cl |= dl;
	buf[7] = bl;
	buf[8] = cl;
	// TODO: disassembled code has a loop apparently XORing the buffer with zeroes, which appears doing nothing
	cl = buf[2];
	dl = buf[5];
	buf[2] = dl;
	dl = buf[8];
	buf[5] = cl;
	cl = buf[4];
	buf[4] = dl;
	dl = buf[7];
	buf[8] = cl;
	cl = buf[6];
	buf[6] = dl;
	buf[7] = cl;
}
