#ifndef ENCODER_H
#define ENCODER_H

#include <string>
#include "base.h"
using namespace std;

class Encoder : public Base
{
    /* we are processing bits but in the end of the day we do I/O in bytes */
    int output_bit_count;
    unsigned long output_bit_buffer;
public:
    Encoder(string inputFile);
private:
    int find_match(unsigned int hash_prefix,unsigned int hash_character);
    void output_code(unsigned int code);
    void compress();
};

#endif // ENCODER_H
