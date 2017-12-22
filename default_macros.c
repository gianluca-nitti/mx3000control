#include "default_macros.h"

#define SCANCODE_CTRL 0xE0
#define SCANCODE_ALT 0xE2

#define MAKE_MACRO(len, ...) {\
	.parse_ok = 1,\
	.length = len,\
	.repeat_count = 1,\
	.keys = {__VA_ARGS__}\
}
#define KEY(a, sc, d) {\
	.delay = d,\
	.action = a,\
	.scancode = sc\
}
#define PRESS(sc) KEY(KEY_DOWN, sc, 0), KEY(KEY_UP, sc, 0)
#define KEYDOWN(scancode) KEY(KEY_DOWN, scancode, 0)
#define KEYUP(scancode) KEY(KEY_UP, scancode, 0)
#define CTRL_KEY(otherKey) MAKE_MACRO(10, KEYDOWN(SCANCODE_CTRL), PRESS(otherKey), KEYUP(SCANCODE_CTRL))
#define ALT_KEY(otherKey) MAKE_MACRO(10, KEYDOWN(SCANCODE_ALT), PRESS(otherKey), KEYUP(SCANCODE_ALT))

const default_macro_t default_macros[] = {
	{"cut", "CTRL+X", CTRL_KEY(0x1B)},
	{"copy", "CTRL+C", CTRL_KEY(0x06)},
	{"paste", "CTRL+V", CTRL_KEY(0x19)},
	{"select-all", "CTRL+A", CTRL_KEY(0x04)},
	{"find", "CTRL+F", CTRL_KEY(0x09)},
	{"new", "CTRL+N", CTRL_KEY(0x11)},
	{"print", "CTRL+P", CTRL_KEY(0x13)},
	{"save", "CTRL+S", CTRL_KEY(0x16)},
	{"switch-window", "ALT+TAB", ALT_KEY(0x2B)},
	{"close-window", "ALT+F4", ALT_KEY(0x3D)},
	{"double-click", NULL, MAKE_MACRO(12, PRESS(0xF0), KEY(KEY_DOWN, 0x01, 1), PRESS(0xF0))}
};

const int numDefaultMacros = sizeof(default_macros) / sizeof(default_macros[0]);
