#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <strings.h>

#define TEST 0
#define TEST_LENGTH_C  100
#define TEST_LENGTH_X  100

#define SIZE 256

void print_bin(unsigned char x);
void print_array(const char* arr, int length);


const int power_of_two[8] = {1, 2, 4, 8, 16, 32, 64, 128};

struct leaf{
		int frq; 			// frequency of char in the input file
		int up;				// number of the upper leaf in the tree
		int left;			// number of left leaf in the tree
		int right;			// number of right leaf in the tree
		int name_length;	// TO DO replace for strlen();
		int code_length;
		char code[8];		// Huffman code of the leaf   // unsigned
		char name[SIZE];
	};

struct hash_code{
		char name;
		int  code_length;
	};


void count_chars(FILE* in_file, int* amount_of_char, long* length){
	char ch;
	printf("Counting chars in input file...\n");
	for (*length = 0; (ch = fgetc(in_file)) != EOF; (*length)++){ // read input file, count amount of every char;
		amount_of_char[(int)ch]++;
	}
	printf("Length of input file is: \t%ld\n", *length);
	printf("Done\n\n");
}

void sort_and_put_chars_in_the_tree(struct leaf* tree, int* amount_of_char){ //sort chars and put in the same order in the leafs of the tree;
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
			tree[i].name[0] = number_of_max_amount;
			tree[i].frq = max_amount;
			tree[i].name_length = 1;
			amount_of_char[number_of_max_amount] = 0;
		}
	}
}

int count_number_of_chars(const struct leaf* tree){ //count number of different chars in the input file
	int result = 0;
	for (int i = 0; i < SIZE; ++i){
		if (tree[i].frq){
			result++;
		}
	}
	printf("Number of chars in the input file is: \t%d\n\n", result);
	return result;
};

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

		tree[current_leaf].frq = tree[min_leaf].frq + tree[pre_min_leaf].frq;
		tree[current_leaf].left = min_leaf;
		tree[current_leaf].right = pre_min_leaf;

		strncat(tree[current_leaf].name, tree[min_leaf].name, tree[min_leaf].name_length);
		strncat(tree[current_leaf].name, tree[pre_min_leaf].name, tree[pre_min_leaf].name_length);
		tree[current_leaf].name_length = strlen(tree[current_leaf].name);

		tree[min_leaf].up = tree[pre_min_leaf].up = current_leaf;
		min_leaf = pre_min_leaf = current_leaf;
	}
}

void check_the_tree(const struct leaf* tree, int number_of_chars, int length){
	int root = 2*number_of_chars - 2;
	printf("Root is: %d\n", root);
	printf("Frq of the root is: \t\t%d\n", tree[root].frq);
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
		int up_code_length = tree[tree[i].up].code_length;
		for (int j = 0; j < up_code_length ; j++){
			tree[i].code[j] = tree[tree[i].up].code[j];
		}
		tree[i].code[up_code_length] = (i == tree[tree[i].up].left) ? 1 : 0;
		tree[i].code_length = up_code_length + 1;
	}
}

void fprint_information_about_tree(FILE* out_file,const struct leaf* tree){
	int out_char_code = 0;
	for (int i = 0; tree[i].name_length == 1; i++){
		out_char_code = 0;
		fprintf(out_file,"%d\t", tree[i].name[0]);
		for (int j = 0; j < tree[i].code_length; j++){
			out_char_code += tree[i].code[j]*power_of_two[7-j];
		}
		fprintf(out_file,"%d\t%d\n", out_char_code, tree[i].code_length);
	}
}

void fprint_pre_data(FILE*in_file, FILE* out_file,long length, int number_of_chars,const struct leaf* tree){
	rewind(in_file); //move to the begun of the input file
	fprintf(out_file, "%ld\n", length); // reserve place for length of output file
	fprintf(out_file,"%d\n", number_of_chars);
	fprint_information_about_tree(out_file, tree);
}


int find_the_leaf_with_name_as_the_char(char ch, const int number_of_chars,const struct leaf* tree){
		for(int i = 0; i < number_of_chars; i++){
			if (ch == tree[i].name[0]){
				return i;
			}
		}
		return -1;
}

void record_to_out_char(int* out_char,int* rest_in_code,int code_length, int* full_in_out_char,const char* code){
	#if TEST
		printf("=========record_to_out_char==========\n");
		printf("full_in_out_char before: %d\n", *full_in_out_char);
		printf("out_char before: ");
		print_bin(*out_char);
		printf("\n\n");


		printf("code of the input char: ");
		print_array(code, code_length);
		printf("\n");
		printf("code_length: %d\n\n", code_length);
		printf("rest in code before: %d\n", *rest_in_code);
		printf("to out char:" );
	#endif

	while ((*rest_in_code < code_length) && (*full_in_out_char < 8)){ //record to out_char until code ends or out_char is full

		if (code[*rest_in_code]){
			*out_char |= power_of_two[7 - *full_in_out_char];

			#if TEST
				printf("%d", 1);
			#endif

		} else {

			#if TEST
				printf("%d", 0);
			#endif
		}
		(*full_in_out_char)++;
		(*rest_in_code)++;
	}

	#if TEST
		printf("\n" );
		printf("rest in code after: %d\n\n", *rest_in_code);
		printf("full_in_out_char after: %d\n", *full_in_out_char);

		printf("out_char after: ");
		print_bin(*out_char);
		printf("\n in integer: %d", *out_char);
		printf("\n=========end==========\n");
	#endif
}

void fprint_post_data(FILE* out_file,int out_char,int full_in_out_char,long* length){
	fprintf(out_file, "%c", out_char);//print the last buffer
	(*length)++;
	fprintf(out_file,"%d", full_in_out_char);//print the number of significant bits in last char
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

struct hash_code* read_hash_tree(FILE* in_file, int number_of_chars){
	int temp_int0 = 0;
	int temp_int1 = 0;
	int temp_int2 = 0;
	struct hash_code* p_hash_tree = calloc(sizeof(struct hash_code), SIZE);
	for (int i = 0; i < number_of_chars; i++){
		fscanf(in_file, "%d\t%d\t%d\n", &temp_int0, &temp_int1, &temp_int2);  // (char, Huffman code of char, length of the code)
		p_hash_tree[temp_int1].name = temp_int0;
		p_hash_tree[temp_int1].code_length = temp_int2;

		// print info about chars and code
		printf("%c\t", p_hash_tree[temp_int1].name);
		printf("%d", temp_int1);
		printf("\t%d\n", p_hash_tree[temp_int1].code_length);
	}

	return p_hash_tree;
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

void read_from_in_char(unsigned char* in_char, char *long_buffer, int *full_in_buffer, int size){
	#if TEST
		puts("=======read_from_char==========");
		printf("Buffer before:");
		print_array(long_buffer, *full_in_buffer);
		printf("\n");
		printf("Full_in_buffer: %d\n", *full_in_buffer);
	#endif

	for (int j = *full_in_buffer + 1; j < *full_in_buffer + size + 1; j++){ //transfer in_char to long_buffer
		long_buffer[2 * (*full_in_buffer) + size - j] = (*in_char) & 1;
		*in_char >>= 1;
	}
	*full_in_buffer += size;

	#if TEST
		printf("Buffer after:");
		print_array(long_buffer, *full_in_buffer);
		printf("\n");
		printf("Full_in_buffer: %d\n", *full_in_buffer);
	#endif
};

void decode_char_and_write(FILE* out_file, int min_length_of_code,int* full_in_buffer, char* long_buffer, struct hash_code* hash_tree){
		char is_found = 0;
		int current_code = 0;
		int leaf_number = 0;
		char anyone_found = 1;
		while (*full_in_buffer > min_length_of_code && anyone_found ){ // try codes from buffer and examine leafs with such code
			for (int j = min_length_of_code; (!is_found) && (j < 8) && (j <= *full_in_buffer); j++){
				current_code = 0;
				for (int k = 0; k < j; k++){
					current_code += long_buffer[k] * power_of_two[7-k];
				}
				if (hash_tree[current_code].code_length == j) {
					leaf_number = current_code;
					is_found = 1;
					anyone_found = 1;
					break;
				} else { anyone_found = 0; }
			}

			if (is_found){
				#if TEST
					printf("Code:");
					print_bin(leaf_number);
					printf("\n");
					printf("Code length: %d\n", hash_tree[leaf_number].code_length);
				#endif
				//shift of the long buffer
				for (int i = 0; i < 16-hash_tree[leaf_number].code_length; i++){
					long_buffer[i] = long_buffer[i + hash_tree[leaf_number].code_length];
				}

			#if TEST
				printf("\t\t\t\t\tOut_char decimal: %d\t hex %x\n",hash_tree[leaf_number].name, hash_tree[leaf_number].name );
			#endif

				fprintf(out_file, "%c", hash_tree[leaf_number].name);
				*full_in_buffer -= hash_tree[leaf_number].code_length;
				leaf_number = 0;
				is_found = 0;
			#if TEST
				printf("Buffer after write out_char:");
				print_array(long_buffer, *full_in_buffer);
				printf("\n");
				printf("Full_in_buffer: %d\n", *full_in_buffer);
			#endif
			}
		}
}

void decoding(FILE* in_file, FILE* out_file, struct hash_code* hash_tree, long length){
	int min_length_of_code = get_min_length_of_code(hash_tree);
	int full_in_buffer = 0;
	int last_full_in_buffer = 8;  // 8 = size of char in bits
	char long_buffer[16] = {0};
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
			last_full_in_buffer = fgetc(in_file) - '0';
			printf("last_full_in_buffer: %d\n", last_full_in_buffer);
		}

		#if TEST
			printf("input char: %x\n", in_char);
			printf("input char: ");
			print_bin(in_char);
			printf("\n\n");
		#endif

		read_from_in_char(&in_char, long_buffer, &full_in_buffer, last_full_in_buffer);
		decode_char_and_write(out_file, min_length_of_code, &full_in_buffer, long_buffer, hash_tree);

	}
	free(hash_tree);
	hash_tree = NULL;
	printf("End of the extract\n");
}

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
//**********************************************************************************************************
//******************************************COMPRESE********************************************************
	if (argv[2][1] == 'c'){

	long length = 0; //input file length
	int amount_of_char[SIZE] = {0};
	count_chars(in_file, amount_of_char, &length);

	struct leaf tree[2*SIZE] = {0};
	sort_and_put_chars_in_the_tree(tree, amount_of_char);

	int number_of_chars = count_number_of_chars(tree); //count number of different chars in the input file

	build_tree(tree, number_of_chars);
	check_the_tree(tree, number_of_chars, length);
	produce_code_for_symbols(tree, (2*number_of_chars - 2) );  // 2*number_of_chars - 2  :this is root leaf in the tree

	//_______________________________________________________record
	printf("Recording to output file...\n");
	fprint_pre_data(in_file, out_file, length, number_of_chars, tree);

	// record input file to the output file
	char in_char;

	int leaf_number;
	int rest_in_code = 0;
	int code_length = 0;

	int full_in_out_char = 0;
	int out_char = 0;

	length = 0;
	while ( (in_char = fgetc(in_file)) != EOF){

		#if TEST
			printf("%s\n", "*********************************************");
			printf("length: %ld\n", length);
			printf("in_char: %x\n", in_char);
			printf("in_char: ");
			print_bin(in_char);
			printf("\n");
		#endif

		leaf_number = find_the_leaf_with_name_as_the_char(in_char, number_of_chars, tree); // add check for -1
		code_length = tree[leaf_number].code_length;
		record_to_out_char(&out_char, &rest_in_code, code_length, &full_in_out_char, tree[leaf_number].code);
		if (full_in_out_char == 8){ // if buffer full, record to the output file

			#if TEST
				printf("wright out_char: %x\n\n", out_char);
			#endif

			fprintf(out_file, "%c", out_char);
			length++;
			out_char = 0;
			full_in_out_char = 0;

			if (rest_in_code < code_length){ // record rest of the code of the current char in buffer
				record_to_out_char(&out_char, &rest_in_code, code_length, &full_in_out_char, tree[leaf_number].code);
			}
		}
		rest_in_code = 0;

	#if TEST
		if (length == TEST_LENGTH_X){break;}
	#endif
	}

	fprint_post_data(out_file, out_char, full_in_out_char, &length);
	puts("Compression is over\n");
	}

//**********************************************************************************************************
//******************************************EXTRACT********************************************************
	if (argv[2][1] == 'x'){

		long length = read_length(in_file);
		int number_of_chars = read_number_of_chars(in_file);
		struct hash_code* hash_tree = read_hash_tree(in_file, number_of_chars);
		decoding(in_file, out_file, hash_tree, length);
	}

	fclose(in_file);
	fclose(out_file);

	time_t timer2 = time(NULL);
	printf("Total time: %lf", difftime(timer2, timer1));
	return 0;
}
