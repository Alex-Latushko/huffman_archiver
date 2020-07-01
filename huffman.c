#include "huffman.h"

#define TEST 0

#if TEST
	#include "test_func.h"
#endif

const int power_of_two[8] = {1, 2, 4, 8, 16, 32, 64, 128};

void count_chars(FILE* in_file, int* amount_of_char, long* length){
	char ch;
	printf("Counting chars in input file...\n");
	for (*length = 0; (ch = fgetc(in_file)) != EOF; (*length)++){ // read input file, count amount of every char;
		amount_of_char[(int)ch]++;
	}
	printf("Length of input file is: \t%ld\n", *length);
	printf("Done\n\n");
}

int count_number_of_chars(const int* amount_of_char){ //count number of different chars in the input file
	int result = 0;
	for (int i = 0; i < SIZE; ++i){
		if (amount_of_char[i]){
			result++;
		}
	}
	printf("Number of chars in the input file is: \t%d\n\n", result);
	return result;
};

void sort_and_put_chars_in_the_tree(struct leaf* tree, int* amount_of_char){ //descending sort of chars and put them in the leafs of the tree;
	int max_amount = 1;
	int number_of_max_amount = 0;
	for (int i = 0; i < SIZE; i++){
		max_amount = 1;
		number_of_max_amount = 0;
		for (int j = 0; j < SIZE; j++){
			if (amount_of_char[j] > max_amount){
				max_amount = amount_of_char[j];
				number_of_max_amount = j;
			}
		}
		if (max_amount > 1){
			tree[i].name[0] = (char)number_of_max_amount;
			tree[i].frq = max_amount;
			tree[i].name_length = 1;
			amount_of_char[number_of_max_amount] = 0;
		}
	}
}

void build_tree(struct leaf* tree, int number_of_chars){
	int min_leaf = 0;
	int pre_min_leaf = 0;
	int current_leaf = 0;
	for (int i = 0; i+1 < number_of_chars; i++){
		for (int j = 0; j < 2*number_of_chars; j++){
			if ( (tree[j].frq) && (tree[j].up == 0) && (tree[j].frq < tree[min_leaf].frq) ){
				min_leaf = j;
			}
		}

		for (int j = 0; j < 2*number_of_chars; j++){
			if ( (tree[j].frq) && (tree[j].up == 0) && (tree[j].frq < tree[pre_min_leaf].frq) && (j != min_leaf)){
				pre_min_leaf = j;
			}
		}
		current_leaf = number_of_chars + i;

		create_leaf_info(tree, current_leaf, pre_min_leaf, min_leaf);
		min_leaf = pre_min_leaf = current_leaf;
	}
}

void create_leaf_info(struct leaf* tree, int current_leaf, int pre_min_leaf, int min_leaf){
	tree[current_leaf].frq = tree[min_leaf].frq + tree[pre_min_leaf].frq;
	tree[current_leaf].left = min_leaf;
	tree[current_leaf].right = pre_min_leaf;

	strncat(tree[current_leaf].name, tree[min_leaf].name, tree[min_leaf].name_length);
	strncat(tree[current_leaf].name, tree[pre_min_leaf].name, tree[pre_min_leaf].name_length);
	tree[current_leaf].name_length = strlen(tree[current_leaf].name);

	tree[min_leaf].up = tree[pre_min_leaf].up = current_leaf;
}

void check_the_tree(const struct leaf* tree, int number_of_chars, int length){
	int root = 2*number_of_chars - 2;
	printf("Root is: %d\n", root);
	printf("Frq of the root is: \t\t%ld\n", tree[root].frq);
	printf("Length of the name of the root is: \t%d\n", tree[root].name_length);

	printf("Length of the input file is equal to frequency of the root: ");
	if (length == tree[root].frq){
			printf("TRUE\n");
		} else {
			printf("FALSE\n");
		}

	printf("Number of chars in the input file is equal to length of the name of the root: ");
	if (number_of_chars == tree[root].name_length){
		printf("TRUE\n");
	} else {
		printf("FALSE\n");
	}
}


void produce_code_for_symbols(struct leaf* tree, int root){
	for (int i = root - 1; i >= 0; i--){
		int up_code_length = tree[tree[i].up].code.length;
		for (int j = 0; j < up_code_length ; j++){
			tree[i].code.value[j] = tree[tree[i].up].code.value[j];
		}
		tree[i].code.value[up_code_length] = (i == tree[tree[i].up].left) ? 1 : 0;
		tree[i].code.length = up_code_length + 1;
	}
}

void fprint_pre_data(FILE*in_file, FILE* out_file, long length, int number_of_chars,const struct leaf* tree){
	rewind(in_file); //move to the begun of the input file
	fprintf(out_file, "%ld\n", length); // reserve place for length of output file
	fprintf(out_file,"%d\n", number_of_chars);
	fprint_information_about_tree(out_file, tree);
}

void fprint_information_about_tree(FILE* out_file,const struct leaf* tree){
	int out_char_code = 0;
	for (int i = 0; tree[i].name_length == 1; i++){
		out_char_code = 0;
		fprintf(out_file,"%d\t", tree[i].name[0]);
		for (int j = 0; j < tree[i].code.length; j++){
			out_char_code += tree[i].code.value[j]*power_of_two[7-j];
		}
		fprintf(out_file,"%d\t%d\n", out_char_code, tree[i].code.length);
	}
}

void fprint_main_data(FILE*in_file, FILE* out_file, long* length, int number_of_chars, const struct leaf* tree){
	*length = 0;

	char in_char;
	int leaf_number = 0;
	struct Code code = { 0, 0, {0}};
	struct char_str out_char = {0, 0};
	while ( (in_char = fgetc(in_file)) != EOF){

		#if TEST
			print_info_fprint_main_data(in_char, *length);
		#endif

		leaf_number = find_the_leaf_with_name_as_the_char(in_char, number_of_chars, tree); // add check for -1
		code = tree[leaf_number].code;
		record_to_out_char(&out_char, &code);
		if (out_char.recorded_bits == 8){ // if buffer full, record to the output file
			fprintf_out_char(out_file, &out_char, length);

			if (code.readed_bits < code.length){ // record rest of the code of the current char in buffer
				record_to_out_char(&out_char, &code);
			}
		}

		#if TEST
			if (*length == TEST_LENGTH_X){break;}
		#endif
	}
	fprint_post_data(out_file, out_char, length);
}

int find_the_leaf_with_name_as_the_char(char ch, const int number_of_chars,const struct leaf* tree){
		for(int i = 0; i < number_of_chars; i++){
			if (ch == tree[i].name[0]){
				return i;
			}
		}
		return -1;
}

void fprintf_out_char(FILE* out_file,struct char_str* out_char, long* length){
	#if TEST
		printf("wrighted out_char: %x\n\n", out_char->value);
	#endif

	fprintf(out_file, "%c", out_char->value);
	(*length)++;
	out_char->value = 0;
	out_char->recorded_bits = 0;
}

void record_to_out_char(struct char_str* out_char, struct Code* code){
	#if TEST
		print_info_record_to_out_char_1(out_char, code);
	#endif

	while ((code->readed_bits < code->length) && (out_char->recorded_bits < 8)){ //record to out_char until code ends or out_char is full

		if (code->value[code->readed_bits]){
			out_char->value |= power_of_two[7 - out_char->recorded_bits];
		}
		#if TEST
			printf("%d", code->value[code->readed_bits]);
		#endif
		(out_char->recorded_bits)++;
		(code->readed_bits)++;
	}

	#if TEST
	print_info_record_to_out_char_2(out_char, code);
	#endif
}

void fprint_post_data(FILE* out_file,struct char_str out_char, long* length){
	fprintf(out_file, "%c", out_char.value);//print the last buffer
	(*length)++;
	fprintf(out_file,"%d", out_char.recorded_bits);//print the number of significant bits in last char
	(*length)++;

	//print length of the output
	rewind(out_file); //move to the beginning of the file
	fprintf(out_file, "%ld\n", *length);  // record actual length of output file
	printf("Length of output: %ld\n", *length);
}

//////////////////////////////////////////extraction functions

long read_length(FILE* in_file){
	long length = 0;
	fscanf(in_file, "%ld\n", &length);
	printf("Length of input: %ld\n", length);
	return length;
}

int read_number_of_chars(FILE* in_file){
	int number_of_chars = 0;
	fscanf(in_file,"%d\n", &number_of_chars);
	printf("Number of chars: %d\n", number_of_chars);
	return number_of_chars;
}

struct hash_code* read_hash_table(FILE* in_file, int number_of_chars){
	int temp_char = 0;
	int temp_code = 0;
	int temp_code_length = 0;
	struct hash_code* p_hash_table = calloc(sizeof(struct hash_code), SIZE);
	for (int i = 0; i < number_of_chars; i++){
		fscanf(in_file, "%d\t%d\t%d\n", &temp_char, &temp_code, &temp_code_length);
		p_hash_table[temp_code].name = temp_char;
		p_hash_table[temp_code].code_length = temp_code_length;

		// print info about chars and code
		printf("%c\t", p_hash_table[temp_code].name);
		printf("%d", temp_code);
		printf("\t%d\n", p_hash_table[temp_code].code_length);
	}

	return p_hash_table;
}

int get_min_length_of_code(const struct hash_code* hash_tree){
	int min_length = 8;
	for (int i = 0; i < SIZE; i++){
		if (hash_tree[i].code_length != 0){
			if (hash_tree[i].code_length < min_length){
				min_length = hash_tree[i].code_length;
			}
		}
	}
	return min_length;
}

void read_from_in_char_to_buffer(unsigned char* in_char, struct Buffer* buffer, int significant_bits_in_input){
	#if TEST
		puts("=======read_from_char==========");
		print_info_buffer(buffer, "before");
	#endif

	for (int j = 0; j < significant_bits_in_input; ++j){ //transfer in_char to long_buffer
		buffer->value[(buffer->recorded_bits) + j] = (*in_char >> (7 - j)) & 1;
	}
	buffer->recorded_bits += significant_bits_in_input;

	#if TEST
		print_info_buffer(buffer, "after");
	#endif
};

void decode_char_and_write(FILE* out_file, struct Buffer* buffer, struct hash_code* hash_table, int min_length_of_code){
	char is_found = 0;
	int current_code = 0;
	int leaf_number = 0;
	char checked_all_table = 0;
	while (buffer->recorded_bits > min_length_of_code && !checked_all_table ){ // try codes from buffer and examine leafs with such code
		current_code = 0;
		for (int k = 0; k < min_length_of_code ; ++k){
			current_code += buffer->value[k] * power_of_two[7-k];
		}
		for (int j = min_length_of_code; !is_found && (j < 8) && (j <= buffer->recorded_bits); ++j){
			if (hash_table[current_code].code_length == j) {
				leaf_number = current_code;
				is_found = 1;
				checked_all_table = 0;
			} else {
				current_code += buffer->value[j] * power_of_two[7-j];
				checked_all_table = 1;
			}
		}

		if (is_found){
			#if TEST
				printf("Code:");
				print_bin(leaf_number);
				printf("\n");
				printf("Code length: %d\n", hash_table[leaf_number].code_length);
			#endif
			//shift of the long buffer
			buffer->recorded_bits -= hash_table[leaf_number].code_length;
			for (int i = 0; i < buffer->recorded_bits; i++){
				buffer->value[i] = buffer->value[i + hash_table[leaf_number].code_length];
			}

		#if TEST
			printf("\t\t\t\t\tOut_char decimal: %d\t hex %x\n",hash_table[leaf_number].name, hash_table[leaf_number].name );
		#endif

			fprintf(out_file, "%c", hash_table[leaf_number].name);
			leaf_number = 0;
			is_found = 0;
		#if TEST
			print_info_buffer(buffer, "after write out_char");
		#endif
		}
	}
}

void decoding(FILE* in_file, FILE* out_file, struct hash_code* hash_tree, long length){
	int min_length_of_code = get_min_length_of_code(hash_tree);

	int significant_bits_in_input = 8 * sizeof(char);
	struct Buffer buffer = {0, {0}};
	unsigned char in_char = 0;

	printf("Decoding...\n");

	#if TEST
		length = TEST_LENGTH_C;
	#endif

	for (long i = length; i > 0; i--){
		#if TEST
			puts("*************************************************");
			printf("Length: %ld\n", (length - i));
		#endif

		if (i > 2){
			in_char = fgetc(in_file);
		} else if (i == 2){
			in_char = fgetc(in_file);
			continue;
		} else if (i == 1 && !TEST){
			significant_bits_in_input = fgetc(in_file) - '0';
			printf("last significant_bit_in_input: %d\n", significant_bits_in_input);
		}

		#if TEST
			printf("input char: %x\n", in_char);
			printf("input char: ");
			print_bin(in_char);
			printf("\n\n");
		#endif

		read_from_in_char_to_buffer(&in_char, &buffer, significant_bits_in_input);
		decode_char_and_write(out_file, &buffer, hash_tree, min_length_of_code);
	}

	free(hash_tree);
	hash_tree = NULL;
	printf("End of the extract\n");
}

int main(int argc, char **argv) {
	time_t timer1 = time(NULL);

	#if TEST
		system("cls");
		puts("TEST MODE ON!");
	#endif

	FILE* in_file = fopen(argv[1], "rb");
	if (in_file != NULL){
		printf("Open input file: %s\n", argv[1]);
	} else {
		printf("There is no such file: %s\n", argv[1]);
		exit(1);
	}

	FILE* out_file = fopen(argv[3], "wb");
	if (out_file != NULL){
		printf ("Open output file:  %s\n", argv[3]);
	} else {
		printf("There is no such file: %s\n", argv[3]);
		exit(1);
	}

	if (argv[2][1] == 'c'){ //************************************COMPRESE**************************************************

		long length = 0; //input file length
		int amount_of_char[SIZE] = {0};
		count_chars(in_file, amount_of_char, &length);
		int number_of_chars = count_number_of_chars(amount_of_char); //count number of different chars in the input file

		struct leaf* tree = calloc(2*number_of_chars, sizeof(struct leaf));
		sort_and_put_chars_in_the_tree(tree, amount_of_char);

		build_tree(tree, number_of_chars);
		check_the_tree(tree, number_of_chars, length);
		produce_code_for_symbols(tree, (2*number_of_chars - 2) );  // 2*number_of_chars - 2  :this is root leaf in the tree

		printf("Recording to output file...\n");
		fprint_pre_data(in_file, out_file, length, number_of_chars, tree);
		fprint_main_data(in_file, out_file, &length, number_of_chars, tree);

		free(tree);
		tree = NULL;

		puts("Compression is over\n");
	}

	if (argv[2][1] == 'x'){ //*************************************EXTRACT***************************************

		long length = read_length(in_file);
		int number_of_chars = read_number_of_chars(in_file);
		struct hash_code* hash_table = read_hash_table(in_file, number_of_chars);
		decoding(in_file, out_file, hash_table, length);
	}

	fclose(in_file);
	fclose(out_file);

	time_t timer2 = time(NULL);
	printf("Total time: %lf", difftime(timer2, timer1));
	printf("\a");
	return 0;
}
