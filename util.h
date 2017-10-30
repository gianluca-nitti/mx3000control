#ifndef _UTIL_H
#define _UTIL_H

int send_feature_report(hid_device* dev, unsigned char* data);
int encode_and_send_feature_report(hid_device* dev, unsigned char* data);
int get_index(const char* str, const char** options, int n_options);
int get_index_or_atoi(const char* str, const char** options, const char* setting_name);
int execute_simple_command(int argc, char** argv, hid_device* dev, const char** options, unsigned char command_byte, const char* setting_name);

#endif
