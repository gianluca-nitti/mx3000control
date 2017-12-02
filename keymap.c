#include "key_config_t.h"

const key_config_t keys[] = {
	{"click", 0, 0xF0, 0, 0},
	{"middle-button", 0, 0xF2, 0, 0},
	{"menu", 0, 0xF1, 0, 0},
	// {"double-click", 0x53, 0, 0x1, 0xA}, //same pattern is used for cut, copy, etc. probably means something like "use macro"
	{"backward", 0, 0xF4, 0, 0},
	{"forward", 0, 0xF3, 0, 0},
	{"dpi-up", 0x71, 0x1, 0, 0},
	{"dpi-down", 0x71, 0x2, 0, 0},
	{"led-color-switch", 0x72, 0x1, 0, 0},
	{"media-play-pause", 0x4, 0, 0, 0xCD},
	{"media-stop", 0x4, 0, 0, 0xB7},
	{"media-previous", 0x4, 0, 0, 0xB6},
	{"media-next", 0x4, 0, 0, 0xB5},
	{"volume-up", 0x4, 0, 0, 0xE9},
	{"volume-down", 0x4, 0, 0, 0xEA},
	{"volume-mute", 0x4, 0, 0, 0xE2},
	{"disable", 0, 0, 0, 0}
};
const int numKeys = sizeof(keys) / sizeof(keys[0]);
