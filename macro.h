#ifndef _MACRO_H
#define _MACRO_H

#include <stdint.h>
#include <hidapi.h>

typedef enum {
	KEY_DOWN,
	KEY_UP
} macro_action;

typedef struct {
	uint8_t delay;
	macro_action action;
	unsigned char scancode;
} macro_entry_t;

typedef struct {
	int parse_ok;
	uint8_t length;
	uint16_t repeat_count;
	macro_entry_t keys[47];
} macro_t;

macro_t parse_macro(char*);
void send_macro(hid_device*, macro_t);

#endif
