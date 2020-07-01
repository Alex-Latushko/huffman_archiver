#include "test_func.h"

void print_bin(unsigned char x){
	int arr[8] = {0};
	for (int i = 0; i < 8; ++i){
		arr[7 - i] = x % 2;
		x /= 2;
	}
	for (int i = 0; i < 8; ++i){
		printf("%d", arr[i] > 0 ? arr[i] : -arr[i]);
	}
}

void print_array(const char* arr, int length){
	for (int i = 0; i < length; ++i){
		printf("%d", arr[i]);
	}
}

void print_info_record_to_out_char_1(struct char_str* out_char, struct Code* code){
	printf("=========record_to_out_char==========\n");
	printf("recorded_bits in out_char before: %d\n", out_char->recorded_bits);
	printf("out_char value before: ");
	print_bin(out_char->value);
	printf("\n\n");

	printf("code of the input char: ");
	print_array(code->value, code->length);
	printf("\n");
	printf("code length: %d\n\n", code->length);
	printf("recorded_bits in code before: %d\n", code->readed_bits);
	printf("to out char:" );
}

void print_info_record_to_out_char_2(struct char_str* out_char, struct Code* code){
	printf("\n" );
	printf("recorded_bits in code after: %d\n\n", code->readed_bits);
	printf("recorded_bits in out_char after: %d\n", out_char->recorded_bits);

	printf("out_char value after: ");
	print_bin(out_char->value);
	printf("\n in integer: %d", out_char->value);
	printf("\n=========end==========\n");
}

void print_info_fprint_main_data(char in_char, long length){
	printf("%s\n", "*********************************************");
	printf("length: %ld\n", length);
	printf("in_char: %x\n", in_char);
	printf("in_char: ");
	print_bin(in_char);
	printf("\n");
}

void print_info_buffer(struct Buffer* buffer, char* str){
	printf("Buffer %s: ", str);
	print_array(buffer->value, buffer->recorded_bits);
	printf("\n");
	printf("Recorded_bits in buffer: %d\n", buffer->recorded_bits);
}





