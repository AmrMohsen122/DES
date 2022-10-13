#include <iostream>
#include <bitset>
#include <stdio.h>
#include <fstream>
/******************************************************* COMMON MACROS ********************************************************************/
#define GET_BIT(VAR,BIT_NUMBER) (((VAR) & (1ULL << (BIT_NUMBER))) >> (BIT_NUMBER))
#define INSERT_BIT(VAR,BIT_VAL,BIT_NUMBER) (VAR = ((VAR & ~(1ULL << BIT_NUMBER)) | ((uint64_t)(BIT_VAL) << BIT_NUMBER)))
/*
THIS MACRO IS NOT GENERIC AND ONLY INTENDED FOR THE FOLLOWING USECASE
LEFT CIRCULAR SHIFT OF 32-BIT VARIABLE THAT IS USED TO CARRY A 28-BIT NUMBER
USE WITH CARE
*/
#define LEFT_CIRCULAR_SHIFT(VAR,SHIFT_NUMBER) ((VAR) = (((VAR) << (SHIFT_NUMBER)) & 0x0FFFFFFF)| ((VAR) >> (28 - (SHIFT_NUMBER))))
#define SWAP_32_BITS(VAR) ((VAR) = ((VAR) << 32) | ((VAR) >> 32))
#define BIT_IS_SET(VAR, BIT_NUMBER) ((VAR) & ( 1ULL << (BIT_NUMBER ) ) )
#define SET_BIT(VAR , BIT_NUMBER) ((VAR) |= (1ULL << (BIT_NUMBER)))
/*****************************************************************************************************************************************/

/******************************************************* DEFINITIONS ********************************************************************/
#define PER_CHOICE_1_SIZE		56
#define PER_CHOICE_2_SIZE		48
#define PER_INITIAL_SIZE		64
#define PER_INVERSE_SIZE		64
#define DES_EXPANSION_SIZE		48
#define F_PERMUTATION_SIZE		32
#define PERMUTE_VAR_SIZE		64
/****************************************************************************************************************************************/

/******************************************************* ENUMS **************************************************************************/

enum word_size {
	_64_BIT_WORD, _56_BIT_WORD
};

/****************************************************************************************************************************************/

/******************************************************* GLOBAL VARIABLES ***************************************************************/
uint8_t init_permumtation_table[PER_INITIAL_SIZE] = {
	58, 50, 42, 34, 26, 18, 10, 2,
	60, 52, 44, 36, 28, 20, 12, 4,
	62, 54, 46, 38, 30, 22, 14, 6,
	64, 56, 48, 40, 32, 24, 16, 8,
	57, 49, 41, 33, 25, 17,  9, 1,
	59, 51, 43, 35, 27, 19, 11, 3,
	61, 53, 45, 37, 29, 21, 13, 5,
	63, 55, 47, 39, 31, 23, 15, 7
};
const uint8_t s_table[8][4][16] = {
   {
   {14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7},
   { 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8},
   { 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0},
   {15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13},
   },

   {
   {15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10},
   { 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5},
   { 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15},
   {13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9},
   },

   {
   {10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8},
   {13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1},
   {13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7},
   { 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12},
   },

   {
   { 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15},
   {13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9},
   {10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4},
   { 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14},
   },

   {
   { 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9},
   {14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6},
   { 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14},
   {11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3},
   },

   {
   {12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11},
   {10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8},
   { 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6},
   { 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13},
   },

   {
   { 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1},
   {13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6},
   { 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2},
   { 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12},
   },

   {
   {13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7},
   { 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2},
   { 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8},
   { 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11},
   },
};
uint8_t inverse_init_permumtation_table[PER_INVERSE_SIZE] = {
	40,  8, 48, 16, 56, 24, 64, 32,
	39,  7, 47, 15, 55, 23, 63, 31,
	38,  6, 46, 14, 54, 22, 62, 30,
	37,  5, 45, 13, 53, 21, 61, 29,
	36,  4, 44, 12, 52, 20, 60, 28,
	35,  3, 43, 11, 51, 19, 59, 27,
	34,  2, 42, 10, 50, 18, 58, 26,
	33,  1, 41,  9, 49, 17, 57, 25
};
uint8_t permutation_choice_1[PER_CHOICE_1_SIZE] = {
   57, 49, 41, 33, 25, 17,  9,
	1, 58, 50, 42, 34, 26, 18,
   10,  2, 59, 51, 43, 35, 27,
   19, 11,  3, 60, 52, 44, 36,
   63, 55, 47, 39, 31, 23, 15,
	7, 62, 54, 46, 38, 30, 22,
   14,  6, 61, 53, 45, 37, 29,
   21, 13,  5, 28, 20, 12,  4
};
uint8_t permutation_choice_2[PER_CHOICE_2_SIZE] = {
   14, 17, 11, 24,  1,  5,
	3, 28, 15,  6, 21, 10,
   23, 19, 12,  4, 26,  8,
   16,  7, 27, 20, 13,  2,
   41, 52, 31, 37, 47, 55,
   30, 40, 51, 45, 33, 48,
   44, 49, 39, 56, 34, 53,
   46, 42, 50, 36, 29, 32
};
uint8_t des_expansion[DES_EXPANSION_SIZE] = {
	32,  1,  2,  3,  4,  5,  4,  5,
	 6,  7,  8,  9,  8,  9, 10, 11,
	12, 13, 12, 13, 14, 15, 16, 17,
	16, 17, 18, 19, 20, 21, 20, 21,
	22, 23, 24, 25, 24, 25, 26, 27,
	28, 29, 28, 29, 30, 31, 32,  1
};
uint8_t	F_permutation[F_PERMUTATION_SIZE] = {
	16,  7, 20, 21, 29, 12, 28, 17,
	 1, 15, 23, 26,  5, 18, 31, 10,
	 2,  8, 24, 14, 32, 27,  3,  9,
	19, 13, 30,  6, 22, 11,  4, 25
};
/*THIS PATHS ARE HARD CODED, REMEMBER TO CHANGE THE MAIN TO TAKE THE PATHS ARE INPUTS*/
char folderPath[150] =
		"E:\\Others\\El koleya el weskha\\Last year\\First term\\sample2.txt";
char folder2Path[150] =
		"E:\\Others\\El koleya el weskha\\Last year\\First term\\outputSample.txt";

//NOTE: add second backslash to the path

/****************************************************************************************************************************************/

/******************************************************* FUNCTION PROTOTYPES *************************************************************/
uint64_t permute_1D(const uint8_t permutation_table[], uint64_t variable, uint8_t size);
void split_word(uint64_t word, uint32_t& l_half_word, uint32_t& r_half_word, enum word_size);
void generate_keys(uint64_t original_key, uint64_t key_container[]);
uint32_t s_box(uint64_t num);
uint8_t extract_col(uint8_t num_6);
uint8_t extract_row(uint8_t x);
uint32_t round_function(uint32_t right_half_word, uint64_t scheduled_key);
uint64_t des_forward_round(uint64_t data, uint64_t scheduled_key);
uint64_t des_inverse_round(uint64_t data, uint64_t scheduled_key);
uint64_t encrypt(uint64_t plain_text, uint64_t key_container[]);
uint64_t decrypt(uint64_t cipher, uint64_t key_container[]);
void read_and_write_in_file(void);
uint8_t realDecimalFromHex(uint8_t c);
/****************************************************************************************************************************************/


/******************************************************* MAIN *************************************************************/

/*
	TESTCASES:
	KEY: 0x133457799BBCDFF1
	MESSAGE: 0x0123456789ABCDEF
	OUTPUT OF INTIIAL PERM: 0xCC00CCFFF0AAF0AA
	OUTPUT OF PERM_CHOICE_1 = 0b11110000110011001010101011110101010101100110011110001111
	C1D1 = 0b11100001100110010101010111111010101011001100111100011110
	perm2 = 0b000110110000001011101111111111000111000001110010
	EXPANSION INPUT: 0xF0AAF0AA
	EXPANSION OUTPUT: 0x7A15557A1555
	PERM_INPUT = 0x5C82B597
	PERM_OUTPUT = 0x234AA9BB
*/

int main(int argc , char* argv[]) {

	// /*LSB----MSB*/
	uint64_t key_container[16] = { 0 };
	uint64_t message = 0x0123456789ABCDEF;
	uint64_t cipherr = 0x56cc09e7cfdc4cef;
	uint64_t key = 0x0123456789ABCDEF;
	//uint64_t key = 0x133457799BBCDFF1;
	generate_keys(key, key_container);
	//for (uint64_t k : key_container) {
	//	std::cout << std::hex << (k) << std::endl;
	//}
	uint64_t cipher = decrypt(cipherr, key_container);
	std::cout <<"cipher " << std::hex << cipher << std::endl;
	return 0;
}

/****************************************************************************************************************************************/



/******************************************************* FUNCTION DEFINITIONS *************************************************************/


uint8_t extract_col(uint8_t num_6) {
	uint8_t mask = 0x21;
	return (num_6 & ~mask) >> 1;
};

uint8_t extract_row(uint8_t x) {

	return ((x & 0x01) | ((x >> 4) & 0x02));
};

uint32_t s_box(uint64_t num) {
	uint64_t temp = num;
	uint64_t result = 0;
	uint8_t var_6 = 0;
	uint32_t res_temp = 0;
	for (int i = 0; i < 8; i++) {
		var_6 = (uint8_t)temp & 0x3F;
		temp >>= 6;
		res_temp = s_table[7 - i][extract_row(var_6)][extract_col(var_6)];
		res_temp <<= (4 * i);
		result |= res_temp;
	}
	return result;
};


/*LSB ----MSB*/
/*
	0x3123456700000000
*/
/*IF THE NUMBER IS LESS 64 BYTES YOU NEED TO SHIFT IT SO THE ZEROS ARE ON THE RIGHT*/
uint64_t permute_1D(const uint8_t permutation_table[], uint64_t variable, uint8_t size_table) {
	uint64_t result = 0;
	for (int i = 0; i < size_table; i++) {
		if (BIT_IS_SET(variable, (PERMUTE_VAR_SIZE - permutation_table[i]))) {
			SET_BIT(result, (PERMUTE_VAR_SIZE - i - 1));
		}
	}
	/*this will return output in the top part of result in case of lower number of outputs the output should be shiffted*/
	return result;
}

void split_word(uint64_t word, uint32_t& l_half_word, uint32_t& r_half_word, enum word_size wordSize) {
	/*half_word1 contains the most 32bit of word
	  half_word2 contains the Least 32bit of word
	*/
	switch (wordSize) {
	case _64_BIT_WORD:
		l_half_word = (word >> 32);
		r_half_word = word;
		break;
	case _56_BIT_WORD:
		l_half_word = ((word >> 28) & 0x000000000FFFFFFF);
		r_half_word = (word & 0x000000000FFFFFFF);
		break;
	}

}

void generate_keys(uint64_t original_key, uint64_t key_container[]) {
	uint32_t key_upper_half = 0;
	uint32_t key_lower_half = 0;
	uint64_t current_key = 0;
	const uint8_t shifts[16] = { 1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1 };
	original_key = (permute_1D(permutation_choice_1, original_key, PER_CHOICE_1_SIZE) >> 8);
	split_word(original_key, key_upper_half, key_lower_half, _56_BIT_WORD);
	for (int i = 0; i < 16; i++) {
		LEFT_CIRCULAR_SHIFT(key_upper_half, shifts[i]);
		LEFT_CIRCULAR_SHIFT(key_lower_half, shifts[i]);
		/*concatenate the two parts of key for the next permutation*/
		current_key = ((static_cast<uint64_t>(key_upper_half) << 28) | (key_lower_half)) << 8;
		current_key = permute_1D(permutation_choice_2, current_key, PER_CHOICE_2_SIZE);
		key_container[i] = current_key;
	}
}

uint32_t round_function(uint32_t right_half_word, uint64_t scheduled_key) {
	/*the shifts are used because permutations expect a 64bit number with LSB on the left*/
	uint64_t expanded_half_word = permute_1D(des_expansion, static_cast<uint64_t>(right_half_word) << 32, DES_EXPANSION_SIZE);
	/* after perm7A15557A1555*/
	expanded_half_word ^= scheduled_key;
	/* after xor 6117BA866527*/
	expanded_half_word = static_cast<uint64_t>(s_box(expanded_half_word >> 16)) << 32;
	/* after s_box 5C82B597*/
	expanded_half_word = permute_1D(F_permutation, expanded_half_word, F_PERMUTATION_SIZE);
	/* after perm 234AA9BB*/
	return expanded_half_word >> 32;
}

uint64_t des_forward_round(uint64_t data, uint64_t scheduled_key) {
	uint32_t data_left_half = 0;
	uint32_t data_right_half = 0;
	uint32_t round_fun_output = 0;
	split_word(data, data_left_half, data_right_half , _64_BIT_WORD);
	round_fun_output = round_function(data_right_half, scheduled_key);
	return (static_cast<uint64_t>(data_right_half) << 32) | (round_fun_output ^ data_left_half);
}

uint64_t des_inverse_round(uint64_t data, uint64_t scheduled_key) {
	uint32_t data_left_half = 0;
	uint32_t data_right_half = 0;
	uint32_t round_fun_output = 0;
	split_word(data, data_left_half, data_right_half, _64_BIT_WORD);
	round_fun_output = round_function(data_left_half , scheduled_key);
	return (((static_cast<uint64_t>(round_fun_output) ^ data_right_half) << 32) | data_left_half);
}

uint64_t encrypt(uint64_t plain_text , uint64_t key_container[]) {
	plain_text = permute_1D(init_permumtation_table, plain_text, PER_INITIAL_SIZE);
	uint64_t temp_output = plain_text;
	for (uint8_t i = 0; i < 16; i++) {
		temp_output = des_forward_round(temp_output, key_container[i]);
		//std::cout << "round#" << static_cast<uint32_t>(i) << " " << std::hex << temp_output << std::endl;
	}
	SWAP_32_BITS(temp_output);
	temp_output = permute_1D(inverse_init_permumtation_table, temp_output, PER_INVERSE_SIZE);
	return temp_output;
}

uint64_t decrypt(uint64_t cipher, uint64_t key_container[]) {
	cipher = permute_1D(inverse_init_permumtation_table, cipher, PER_INVERSE_SIZE);
	std::cout << std::hex << cipher << std::endl;
	SWAP_32_BITS(cipher);
	uint64_t temp_output = cipher;
	for(char i = 15; i >= 0; --i) {
		temp_output = des_inverse_round(temp_output, key_container[i]);
		std::cout << "round#" << static_cast<uint32_t>(i) << " " << std::hex << temp_output << std::endl;
	}
	temp_output = permute_1D(init_permumtation_table, temp_output, PER_INITIAL_SIZE);
	return temp_output;
}
/*
 * Description: function to read a hex data from file and write the cipher in another file
 * 1)It reads the first 16 chars from the file
 * 2)Convert the ascii value to the real decimal of the hex
 * 2)Store it in variable "data"
 * 3)Process the data and writes the output in file
 *
 * 	input:void
 * 	output:
 * */
void read_and_write_in_file(void) {
	uint64_t dataBuffer = 0;
	FILE *ptr;
	int8_t ch; //Exits the loop when ch=-1 so its type is signed int
	uint8_t realDec, counter = 0; //realDec to convert the ascii value to the real hex
	//counter to count how many iterations in the loop are executed

	// Opening file in reading mode
	ptr = fopen(folderPath, "r");
	std::ofstream outputFile(folder2Path);


	// character by character using loop.
	do {
		counter++;
		ch = fgetc(ptr); //get the character
		realDec = realDecimalFromHex(ch); //convert it
		dataBuffer <<= 4; //shift the data left by 4 as every hex is 4 bits
		dataBuffer |= realDec; //oring with realDec to overrides the zeroes with real data and leave the original ones unchanged
		outputFile << std::hex<<dataBuffer;


		if (counter == 16) { //4*16=64 so the 64 bit of 'data' is consumed
			//TODO : call function to convert it to cipher
			//std::cout << std::hex << dataBuffer << std::endl;
			counter = 0;

		}

		// Checking if character is not EOF.
		// If it is EOF stop reading.
	} while (ch != EOF);

	// Closing the file
	fclose(ptr);
	outputFile.close();

}

//Description: the function converts ascii value to the real hex
//input: char to be converted
//output: real deciaml value of the char
//example: if the char is 'A' it will return 10
uint8_t realDecimalFromHex(uint8_t c) {
	uint8_t value = c;
	if (c >= 'A' && c <= 'F') {
		value = c - 'A' + 10;

	} else if (c >= '0' && c <= '9') {
		value = c - '0';
	}
	return value;
}


/****************************************************************************************************************************************/
