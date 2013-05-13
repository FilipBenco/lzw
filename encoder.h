#ifndef ENCODER_H
#define ENCODER_H

#include <fstream>
#include "base.h"

#define HASHING_SHIFT (BITS-8)

using namespace std;

class Encoder : public Base
{
    ifstream input_file;
    ofstream lzw_file;
    int output_bits_count;
    unsigned long output_bits_buffer;
	int *code_value; 
public:
    Encoder(string inputFile, string outputFile);
	~Encoder();
private:
    void encode();
    int find_code_index(unsigned int hash_prefix,unsigned int hash_character);
    void output_code(unsigned int code);
};

#endif // ENCODER_H
