#include <iostream>
#include <bitset>
/******************************************************* COMMON MACROS ********************************************************************/
#define GET_BIT(VAR,BIT_NUMBER) (((VAR) & ((uint64_t)1 << (BIT_NUMBER))) >> (BIT_NUMBER))
#define INSERT_BIT(VAR,BIT_VAL,BIT_NUMBER) (VAR = ((VAR & ~((uint64_t)1 << BIT_NUMBER)) | ((uint64_t)(BIT_VAL) << BIT_NUMBER)))
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
/****************************************************************************************************************************************/

/******************************************************* ENUMS **************************************************************************/

enum word_size {
	_64_BIT_WORD , _56_BIT_WORD
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
/****************************************************************************************************************************************/

/******************************************************* FUNCTION PROTOTYPES *************************************************************/
uint64_t permute_1D(const uint8_t permutation_table[], uint64_t variable , uint8_t size);

void split_word(uint64_t word, uint32_t& l_half_word, uint32_t& r_half_word, enum word_size);

void generate_keys(uint64_t original_key , uint64_t key_container[]);
/****************************************************************************************************************************************/


/******************************************************* MAIN *************************************************************/
int main(void) {
	/*MSB----LSB*/
	uint64_t key_container[16] = {0};
	//uint64_t message = 0x7123456789ABCDEF;
	uint64_t key = 0x8FFB3DD99EEA2CC8;
	generate_keys(key, key_container);
	//for (int i = 0; i < 64; i++) {
	//	std::cout << GET_BIT(key, i);
	//}
	//std::cout << std::bitset<64>(key) << std::endl;
	//key = permute_1D(permutation_choice_1, key, PER_CHOICE_1_SIZE);
	//std::cout << std::bitset<PER_CHOICE_1_SIZE>(key) << std::endl;
	for (uint64_t k : key_container) {
		//std::cout <<std::bitset<64>(k) << std::endl;
		std::cout <<std::hex << (k) << std::endl;
	}
	return 0;
}

/****************************************************************************************************************************************/



/******************************************************* FUNCTION DEFINITIONS *************************************************************/

uint64_t permute_1D(const uint8_t permutation_table[], uint64_t variable , uint8_t size) {
	uint64_t result = 0;
	for (int i = 0; i < size; i++) {
		if (BIT_IS_SET(variable, (permutation_table[i] - 1))) {
			SET_BIT(result, i);
		}
	}
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
	const uint8_t shifts[16] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};
	original_key = permute_1D(permutation_choice_1, original_key, PER_CHOICE_1_SIZE);
	split_word(original_key, key_upper_half, key_lower_half , _56_BIT_WORD);
	for (int i = 0; i < 16; i++) {
		LEFT_CIRCULAR_SHIFT(key_upper_half , shifts[i]);
		LEFT_CIRCULAR_SHIFT(key_lower_half , shifts[i]);
		/*concatenate the two parts of key for the next permutation*/
		current_key = (static_cast<uint64_t>(key_upper_half) << 28) | (key_lower_half);
		current_key = permute_1D(permutation_choice_2, current_key, PER_CHOICE_2_SIZE);
		key_container[i] = current_key;
	}
}
/****************************************************************************************************************************************/