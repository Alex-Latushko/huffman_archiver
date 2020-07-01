#ifndef TEST_FUNC_H_
#define TEST_FUNC_H_
#include "huffman.h"

void print_bin(unsigned char x);
void print_array(const char* arr, int length);
void print_info_record_to_out_char_1(struct char_str* out_char, struct Code* code);
void print_info_record_to_out_char_2(struct char_str* out_char, struct Code* code);
void print_info_fprint_main_data(char in_char, long length);
void print_info_buffer(struct Buffer* buffer, char* str);

#endif /* TEST_FUNC_H_ */
