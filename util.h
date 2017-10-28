#ifndef _UTIL_H
#define _UTIL_H

int send_feature_report(hid_device* dev, unsigned char* data);
int encode_and_send_feature_report(hid_device* dev, unsigned char* data);
int get_index(char* str, const char** options, int n_options);
int get_index_or_atoi(char* str, const char** options, int n_options, char* message_prefix);

#endif
