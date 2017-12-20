#ifndef _DEFAULT_MACROS_H
#define _DEFAULT_MACROS_H

#include "macro.h"

#define SCANCODE_CTRL 0xE0

#define MAKE_MACRO(len, ...) {\
	.parse_ok = 1,\
	.length = len,\
	.repeat_count = 1,\
	.keys = {__VA_ARGS__}\
}
#define KEY(a, sc) {\
	.delay = 0,\
	.action = a,\
	.scancode = sc\
}
#define PRESS(sc) KEY(KEY_DOWN, sc), KEY(KEY_UP, sc)
#define CTRL_DOWN KEY(KEY_DOWN, SCANCODE_CTRL)
#define CTRL_UP KEY(KEY_UP, SCANCODE_CTRL)
#define CTRL_KEY(otherKey) MAKE_MACRO(4, CTRL_DOWN, PRESS(otherKey), CTRL_UP)

typedef struct {
	char* name;
	macro_t macro;
} default_macro_t;

const default_macro_t default_macros[] = {
	{"cut", CTRL_KEY(0x1B)},
	{"copy", CTRL_KEY(0x06)},
	{"paste", CTRL_KEY(0x19)}
};

const int numDefaultMacros = sizeof(default_macros) / sizeof(default_macros[0]);

#endif
