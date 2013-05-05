#include "encoder.h"
#include "decoder.h"
#include <iostream>
int main(int argc, char *argv[])
{
    Encoder encoder = Encoder("test.bmp");
    Decoder decoder = Decoder("output.bmp");
}
