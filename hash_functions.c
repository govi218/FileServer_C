#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"
void show_hash(char *hash_val, long block_size);
/*
 * Function to continuously loop over hash val and xor each character with
 * the corresponding user input character.
 */
char* hash(FILE* file_ptr) {
	int block_size = 8;

	char* hash_val = malloc(sizeof(char) * 1024);
	memset(hash_val, 0, 1024);
	//loop counter
	int i = 0;
	char ch;
	//while Ctrl-D isn't typed, scan user inputted chars and xor with
	//corresponding hash value
	while((ch = fgetc(file_ptr)) != EOF && file_ptr) {
		hash_val[i] = (hash_val[i] ^ ch);
		// if i has reached the block_size, start over; if not, continue
		(i == block_size - 1)? i = 0: i++;
	}
	printf("hash val: %s\n", hash_val);
	return hash_val;
}
/*
 * Compares hash, returns index where they differ. If they don't, return
 * the block_size.
 */
int check_hash(const char *hash1, const char *hash2) {
	//loop counter
	int i = 0;
	//while each character of the hashes are the same and i isn't at the
	//end, increment i
	while(hash1[i] == hash2[i] && i < HASH_SIZE) {
		i++;
	}
	printf("1.%s 2.%s i.%d\n", hash1, hash2, i);
	if (i == HASH_SIZE && (strlen(hash1) == strlen(hash2)))
	{
		return 1;
	}
    return 0;
}
