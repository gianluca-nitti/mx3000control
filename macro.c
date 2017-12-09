#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "macro.h"

#define STRLEN(s) (sizeof(s) / sizeof(s[0]) - 1)

static const char repeat_prefix[] = "repeat-";
static const char delay_prefix[] = "delay-";
static const char keydown_prefix[] = "down";
static const char keyup_prefix[] = "up";

macro_t parse_macro(char* str) {
	macro_t result = {0};
	if (strncmp(str, repeat_prefix, STRLEN(repeat_prefix)) == 0) {
		str += STRLEN(repeat_prefix);
		char* newStr;
		result.repeat_count = (int16_t) strtol(str, &newStr, 10);
		if (newStr == str) {
			fwprintf(stderr, L"Failed to parse macro repeat count\n");
			return result;
		}
		if (result.repeat_count <= 0) {
			fwprintf(stderr, L"Invalid macro repeat number (must be a positive integer)\n");
			return result;
		}
		str = newStr;
		if (*str != '-') {
			fwprintf(stderr, L"Failed to parse macro: expected dash after repeat count\n");
			return result;
		}
		str++;
	}
	wprintf(L"Repeat count: %d, remaining string: %s\n", result.repeat_count, str); //TODO remove
	int key_index = 0;
	while (*str != '\0') {
		if (key_index != 0) {
			if (*str != '-') {
				fwprintf(stderr, L"Failed to parse macro: expected dash before new key data\n");
				return result;
			}
			str++;
		}
		if (strncmp(str, delay_prefix, STRLEN(delay_prefix)) == 0) {
			str += STRLEN(delay_prefix);
			char* newStr;
			result.keys[key_index].delay = (uint8_t) strtol(str, &newStr, 10);
			if (newStr == str) {
				fwprintf(stderr, L"Failed to parse key delay in macro\n");
				return result;
			}
			if ((result.keys[key_index].delay & 0x80) != 0) { //msb must be zero (range is 0-127)
				fwprintf(stderr, L"Macro key delay is out of range\n");
				return result;
			}
			str = newStr;
			if (*str != '-') {
				fwprintf(stderr, L"Failed to parse macro: expected dash after delay value\n");
				return result;
			}
			str++;
		}

		if (strncmp(str, keydown_prefix, STRLEN(keydown_prefix)) == 0) {
			str += STRLEN(keydown_prefix);
			result.keys[key_index].action = KEY_DOWN;
		} else if (strncmp(str, keyup_prefix, STRLEN(keyup_prefix)) == 0) {
			str += STRLEN(keyup_prefix);
			result.keys[key_index].action = KEY_UP;
		} else {
			fwprintf(stderr, L"Failed to parse macro: unrecognized action identifier (up/down)\n");
			return result;
		}

		char* newStr;
		result.keys[key_index].scancode = (unsigned char) strtol(str, &newStr, 16);
		if (newStr == str) {
			fwprintf(stderr, L"Failed to parse macro: action is not followed by a valid keyboard scancode\n");
			return result;
		}

		str = newStr;
		key_index++;
		// TODO: ensure there are no more entries than the max allowed
	}
	result.parse_ok = 1;
	result.length = key_index;
	wprintf(L"Finished parsing macro, length = %d\n", result.length); //TODO remove
	return result;
}

void send_macro(hid_device* dev, macro_t macro) {

}
