#ifndef HUFFMAN_H_
#define HUFFMAN_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <strings.h>


#define TEST_LENGTH_C  100   //amount of input chars in test compression mode
#define TEST_LENGTH_X  100	 //amount of input chars in test extraction mode

#define SIZE 256

struct char_str{
	short recorded_bits;
	int value;
};

struct Buffer{
	short recorded_bits;
	char value[16];
};

struct Code{
	short readed_bits;
	int length;
	char value[8];
};

struct leaf{
		int up;				// number of the upper leaf in the tree
		int left;			// number of left leaf in the tree
		int right;			// number of right leaf in the tree
		int name_length;
		long frq; 			// frequency of char in the input file
		struct Code code;	// Huffman code of the leaf   // unsigned
		char name[SIZE];
};

struct hash_code{
		char name;
		int  code_length;
};

void count_chars(FILE* in_file, int* amount_of_char, long* length);
int count_number_of_chars(const int* amount_of_char);
void sort_and_put_chars_in_the_tree(struct leaf* tree, int* amount_of_char);
void build_tree(struct leaf* tree, int number_of_chars);
void create_leaf_info(struct leaf* tree, int current_leaf, int pre_min_leaf, int min_leaf);
void check_the_tree(const struct leaf* tree, int number_of_chars, int length);
void produce_code_for_symbols(struct leaf* tree, int root);
void fprint_pre_data(FILE*in_file, FILE* out_file, long length, int number_of_chars,const struct leaf* tree);
void fprint_information_about_tree(FILE* out_file,const struct leaf* tree);
void fprint_main_data(FILE*in_file, FILE* out_file, long* length, int number_of_chars, const struct leaf* tree);
int find_the_leaf_with_name_as_the_char(char ch, const int number_of_chars,const struct leaf* tree);
void fprintf_out_char(FILE* out_file,struct char_str* out_char, long* length);
void record_to_out_char(struct char_str* out_char, struct Code* code);
void fprint_post_data(FILE* out_file,struct char_str out_char, long* length);

//////////////////////////////////////////extraction functions
long read_length(FILE* in_file);
int read_number_of_chars(FILE* in_file);
struct hash_code* read_hash_table(FILE* in_file, int number_of_chars);
int get_min_length_of_code(const struct hash_code* hash_tree);
void read_from_in_char_to_buffer(unsigned char* in_char, struct Buffer* buffer, int significant_bits_in_input);
void decode_char_and_write(FILE* out_file, struct Buffer* buffer, struct hash_code* hash_tree, int min_length_of_code);
void decoding(FILE* in_file, FILE* out_file, struct hash_code* hash_tree, long length);



#endif /* HUFFMAN_H_ */
