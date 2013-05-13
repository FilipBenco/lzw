#ifndef DECODER_H
#define DECODER_H

#include <fstream>
#include "base.h"

using namespace std;

class Decoder : public Base
{
    ifstream lzw_file;
    ofstream output_file;
    int input_bit_count;
    unsigned long input_bit_buffer;
	unsigned char decode_stack[4000]; 

public:
    Decoder(string inputFile, string outputFile);
protected:
    unsigned char *decode_string(unsigned char *buffer,unsigned int code);
    void expand();
    int input_code();
};

#endif // DECODER_H
