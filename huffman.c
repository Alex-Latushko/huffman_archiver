#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 256

const int power_of_two[8] = {1, 2, 4, 8, 16, 32, 64, 128};

struct leaf{
		int frq; 			// frequency of char in the input file
		int up;				// number of the upper leaf in the tree
		int left;			// number of left leaf in the tree
		int right;			// number of right leaf in the tree
		int name_length;
		int code_length;
		char code[8];		// Huffman code of the leaf
		char name[SIZE];
	};

struct hash_code{
		char name;
		int  code_length;

	};


void count_chars(FILE* in_file, int* amount_of_char, long* plength){
	char ch;
	printf("Counting chars in input file...\n");
	for (*plength = 0; (ch = fgetc(in_file)) != EOF; (*plength)++){ // read input file, count amount of every char;

		amount_of_char[(int)ch]++;
	}
	printf("Length of input file is: \t%d\n", *plength);
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

int count_number_of_chars(struct leaf* tree){ //count number of different chars in the input file
	int result = 0;
	for (int i = 0; i < SIZE; ++i){
		if (tree[i].frq){
			result++;
		}
	}
	printf("Number of chars in the input file is: \t%d\n\n", result);
	return result;
};

void build_tree(struct leaf* tree, const int number_of_chars){
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
		tree[current_leaf].name_length = tree[min_leaf].name_length + tree[pre_min_leaf].name_length;
		for (int j = 0; j < tree[min_leaf].name_length; j++){
			tree[current_leaf].name[j] = tree[min_leaf].name[j];
		}
		for (int j = tree[min_leaf].name_length; j < tree[current_leaf].name_length; j++){
			tree[current_leaf].name[j] = tree[pre_min_leaf].name[j-tree[min_leaf].name_length];
		}
		tree[min_leaf].up = tree[pre_min_leaf].up = current_leaf;

		min_leaf = pre_min_leaf = current_leaf;
	}
}

void check_the_tree(struct leaf* tree, const int number_of_chars,const int length){
	int root = 2*number_of_chars - 2;
	printf("Root is: %d\n", root);
	printf("Frq of the root is: \t%d\n", tree[root].frq);
	printf("Length of the name of the root is: \t\t%d\n", tree[root].name_length);

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


void produce_code_for_symbols(struct leaf* tree,const int root){
	for (int i = root - 1; i >= 0; i-- ){
		for (int j = 0; j < tree[tree[i].up].code_length; j++){
			tree[i].code[j] = tree[tree[i].up].code[j];
		}
		if (i == tree[tree[i].up].left){
			tree[i].code[tree[tree[i].up].code_length] = 1;
		} else {
			tree[i].code[tree[tree[i].up].code_length] = 0;
		}
		tree[i].code_length = tree[tree[i].up].code_length + 1;
	}
}

void fprint_information_about_tree(FILE* out_file, struct leaf* tree){
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

int find_the_leaf_with_name_as_the_char(char ch, const int number_of_chars, struct leaf* tree){
		int result = 0;
		for(int i = 0; i < number_of_chars; i++){
			if (ch == tree[i].name[0]){
				result = i;
				break;
			}
		}
		return result;
}


//extraction functions

void read_hash_tree(FILE* in_file, struct hash_code* hash_tree, int number_of_chars, int* min_length_of_code){
	int temp_int0 = 0;
	int temp_int1 = 0;
	int temp_int2 = 0;
	for (int i = 0; i < number_of_chars; i++){
		fscanf(in_file, "%d\t%d\t%d\n", &temp_int0, &temp_int1, &temp_int2);  // (char, Huffman code of char, length of the code)
		hash_tree[temp_int1].name = temp_int0;
		hash_tree[temp_int1].code_length = temp_int2;
		if (i == 0) {
			*min_length_of_code = hash_tree[temp_int1].code_length;
		}
		// print info about chars and code
		printf("%c\t", hash_tree[temp_int1].name);
		printf("%d", temp_int1);
		printf("\t%d\n", hash_tree[temp_int1].code_length);
	}
}

void read_from_in_char(unsigned char* in_char, char *long_buffer, int *full_in_buffer, int size){
	for (int j = *full_in_buffer + 1; j < *full_in_buffer + size + 1; j++){ //transfer in_char to long_buffer
		long_buffer[2 * (*full_in_buffer) + size - j] = (*in_char) & 1;
		*in_char >>= 1;
	}
	*full_in_buffer += size;
};

void decode_char_and_write(FILE* out_file, int min_length_of_code,int* full_in_buffer, char* long_buffer, struct hash_code* hash_tree){
		char is_found = 0;
		int current_code = 0;
		int leaf_number = 0;
		while (*full_in_buffer > min_length_of_code){ // try codes from buffer and examine leafs with such code
			for (int j = min_length_of_code; (!is_found) && (j < 8); j++){
				current_code = 0;
				for (int k = 0; k < j; k++){
					current_code += long_buffer[k] * power_of_two[7-k];
				}
				if (hash_tree[current_code].code_length == j) {
					leaf_number = current_code;
					is_found = 1;
					break;
				}
			}


			if (is_found){
				//shift of the long buffer
				for (int j = 0; j < 16-hash_tree[leaf_number].code_length; j++){
					long_buffer[j] = long_buffer[j+hash_tree[leaf_number].code_length];
				}

				fprintf(out_file, "%c", hash_tree[leaf_number].name);
				*full_in_buffer -= hash_tree[leaf_number].code_length;
				leaf_number = 0;
				is_found = 0;
			}
		}
}

int main(int argc, char **argv) {
	time_t timer1 = time(NULL);
	int amount_of_char[SIZE] = {0};
	char ch;
	int leaf_number;
	int full_in_buffer = 0;
	int rest_in_code = 0;

	long length = 0; //file length

	FILE* in_file = fopen(argv[1], "rb");
	if (in_file != NULL){
		printf("Open input file: %s\n", argv[1]);
	}

	FILE* out_file = fopen(argv[3], "wb");
	if (out_file != NULL){
		printf ("Open output file:  %s\n", argv[3]);
	}
//**********************************************************************************************************
//******************************************COMPRESE********************************************************
	if (argv[2][1] == 'c'){

	struct leaf tree[2*SIZE] = {0};

	count_chars(in_file, amount_of_char, &length);
	sort_and_put_chars_in_the_tree(tree, amount_of_char);

	int number_of_chars = count_number_of_chars(tree); //count number of different chars in the input file

	build_tree(tree, number_of_chars);
	check_the_tree(tree, number_of_chars, length);
	produce_code_for_symbols(tree, (2*number_of_chars - 2) );  // 2*number_of_chars - 2  :root leaf in the tree

	//_______________________________________________________record
	printf("Recording to output file...\n");
	rewind(in_file); //move to the begun of the input file
	fprintf(out_file, "%ld\n", length); // here we know only length of input file
	fprintf(out_file,"%d\n", number_of_chars);
	fprint_information_about_tree(out_file, tree);

	// record input file to the output file
	length = 0;
	int out_char = 0;
	int code_length = 0;
	while ( (ch = fgetc(in_file)) != EOF){

		leaf_number = find_the_leaf_with_name_as_the_char(ch, number_of_chars, tree);
		code_length = tree[leaf_number].code_length;

		while ((rest_in_code < code_length) && (full_in_buffer < 8)){ //record to buffer until code ends or buffer is full
			if (tree[leaf_number].code[rest_in_code]){
				out_char |= power_of_two[7-full_in_buffer];
			}
			full_in_buffer++;
			rest_in_code++;
		}

		if (full_in_buffer == 8){ // if buffer full, record to the output file
			fprintf(out_file, "%c", out_char );
			//fputc(out_char, out_file);
			length++;
			out_char = 0;
			full_in_buffer = 0;

			if (rest_in_code < code_length){ // record rest of the code of the current char in buffer
				while (rest_in_code < code_length){
					if (tree[leaf_number].code[rest_in_code]){
						out_char |= power_of_two[7-full_in_buffer];
					}
					full_in_buffer++;
					rest_in_code++;
				}
			}
		}
		rest_in_code = 0;
	}

	fputc(out_char, out_file);//print the last buffer
	length++;
	fprintf(out_file,"%d", full_in_buffer);//print the number of significant bits in last char
	length++;

	//print length of the output
	rewind(out_file); //move to the beginning of the file
	fprintf(out_file, "%ld\n", length);
	printf("Length of output: %ld\n", length);
	fclose(out_file);

	puts("Compression is over\n");
	} //_________________________________________________________ end of compress

	if (argv[2][1] == 'x'){  //_______________________ begin of the extract

		fscanf(in_file, "%ld\n", &length);
		printf("Length of input: %ld\n", length);

		int number_of_chars = 0;
		fscanf(in_file,"%d\n", &number_of_chars);
		printf("Number of chars: %d\n", number_of_chars);

		struct hash_code hash_tree[SIZE];

		int min_length_of_code = 0;
		read_hash_tree(in_file, hash_tree, number_of_chars, &min_length_of_code);

		full_in_buffer = 0;
		int last_full_in_buffer = 8;  // 8 = size of char in bits
		char long_buffer[16] = {0};
		unsigned char in_char = 0;

		printf("Decoding...\n");
		for (long i = length; i > 0; i--){

			if (i > 2){
				in_char = fgetc(in_file);
			} else if (i == 2){
				in_char = fgetc(in_file);
				continue;
			} else if (i == 1){
				last_full_in_buffer = fgetc(in_file) - '0';
				printf("last_full_in_buffer: %d\n", last_full_in_buffer);
			}

			read_from_in_char(&in_char, long_buffer, &full_in_buffer, last_full_in_buffer);
			decode_char_and_write(out_file, min_length_of_code, &full_in_buffer, long_buffer, hash_tree);

		}
		printf("End of the extract\n");
	}

	fclose(in_file);
	fclose(out_file);

	time_t timer2 = time(NULL);
	printf("Total time: %lf", difftime(timer2, timer1));
	return 0;
}