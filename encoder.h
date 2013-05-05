#ifndef ENCODER_H
#define ENCODER_H

#include <string>
#include <fstream>
#include "base.h"
using namespace std;

class Encoder : public Base
{
    ifstream input_file;
    ofstream lzw_file;
    int output_bits_count;
    unsigned long output_bits_buffer;
public:
    Encoder(string input);
private:
    void encode();
    int find_match(unsigned int hash_prefix,unsigned int hash_character);
    void output_code(unsigned int code);
};

#endif // ENCODER_H
