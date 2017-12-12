#ifndef _UTIL_H
#define _UTIL_H

#include <hidapi.h>

typedef enum {
	FEATURE_REPORT,
	OUTPUT_REPORT
} report_type;

int send_report(hid_device*, unsigned char*, report_type);
int encode_and_send_report(hid_device*, unsigned char*, report_type);
int get_index(const char*, const char**, int);
int get_index_or_atoi(const char*, const char**, int, const char*);
int execute_simple_command(int, char**, hid_device*, const char**, int, unsigned char, const char*);

#endif
