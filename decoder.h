#ifndef DECODER_H
#define DECODER_H

#include <string>
#include "base.h"

using namespace std;

class Decoder : public Base
{
    int input_bit_count;
    unsigned long input_bit_buffer;
public:
    Decoder(string outputFile);
protected:
    unsigned char *decode_string(unsigned char *buffer,unsigned int code);
    void expand();
    int input_code();
};

#endif // DECODER_H
