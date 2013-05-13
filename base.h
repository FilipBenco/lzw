#ifndef BASE_H
#define BASE_H

#include <string>
#include <stdlib.h>
#include <limits.h>
#include <string>

#define MIN_CODE_LEN 9
#define MAX_CODE_LEN 20
#define FIRST_CODE (1 << CHAR_BIT)
#define BITS 17
#define TABLE_SIZE 131101
#define HASHING_SHIFT (BITS-8)
#define MAX_CODE (1 << BITS)-2
#define CURRENT_MAX_CODES(x) (1UL << (x))
#define LZW_DETECTION "LZW"

class Base {
protected:
    unsigned int *prefix_code;
    unsigned char *append_character;
    unsigned char current_bits;
public:
    Base() {
        prefix_code=(unsigned int*)malloc(TABLE_SIZE*sizeof(unsigned int));
        append_character=(unsigned char*)malloc(TABLE_SIZE*sizeof(unsigned char));
    }
	~Base() {
		free(this->prefix_code);
		free(this->append_character);
	}
};

#endif // BASE_H

