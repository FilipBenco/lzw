#include "decoder.h"

Decoder::Decoder(string outputFile)
{
    io_error = 1;

    lzw_file=fopen("output.lzw","rb");
    if(lzw_file) {
        /* check LZW identifier L+starting bytes */
        int ch1 = getc(lzw_file);
        int ch2 = getc(lzw_file);
        if('L' == ch1 && MIN_CODE_LEN==ch2) {
            io_file=fopen(outputFile.c_str(),"wb");
            if(io_file) {
                expand();
                io_error = ferror(lzw_file) || ferror(io_file);

                fclose(io_file);
                io_file = 0;
            }
        }

        fclose(lzw_file);
        lzw_file = 0;
    }
}

void Decoder::expand() {
    unsigned int next_code;
    unsigned int new_code;
    unsigned int old_code;
    int character;
    unsigned char *string;
    unsigned int bit_limit;

    ATLASSERT(code_value); /* initialized? */

    CUR_BITS = MIN_CODE_LEN;
    bit_limit = CURRENT_MAX_CODES(CUR_BITS) - 1;
    input_bit_count=0;
    input_bit_buffer=0L;

    // @@@ what if we pass uncompressed file to decode?

    next_code=FIRST_CODE;        /* This is the next available code to define */

    old_code=input_code();       /* Read in the first code, initialize the */
    if(-1 == old_code)
        return; /* read error? */
    character=old_code;          /* character variable, and send the first */
    if(putc_out(old_code)==-1)   /* code to the output file                */
        return; /* write error */
    /*
**  This is the main expansion loop.  It reads in characters from the LZW file
**  until it sees the special code used to inidicate the end of the data.
*/
    while ((new_code=input_code()) != (-1))
    {
        /* look for code length increase marker */
        if(bit_limit == new_code && CUR_BITS < BITS)
        {
            CUR_BITS++;
            bit_limit = CURRENT_MAX_CODES(CUR_BITS) - 1;

            new_code=input_code();
            ATLASSERT(new_code != -1); /* must be read error? */
            if(new_code == -1)
                break;
        }

        ATLASSERT(new_code < bit_limit);

        /*
** This code checks for the special STRING+CHARACTER+STRING+CHARACTER+STRING
** case which generates an undefined code.  It handles it by decoding
** the last code, and adding a single character to the end of the decode string.
*/
        if (new_code>=next_code)
        {
            *decode_stack=character;
            string=decode_string(decode_stack+1,old_code);
        }
        /*
** Otherwise we do a straight decode of the new code.
*/
        else
            string=decode_string(decode_stack,new_code);
        /*
** Now we output the decoded string in reverse order.
*/
        character=*string;
        while (string >= decode_stack)
            putc_out(*string--);
        /*
** Finally, if possible, add a new code to the string table.
*/
        if (next_code <= MAX_CODE)
        {
            prefix_code[next_code]=old_code;
            append_character[next_code]=character;
            next_code++;
        }
        old_code=new_code;
    }
}

int Decoder::input_code() {
    int c;
    unsigned int return_value;
    //static int input_bit_count=0;
    //static unsigned long input_bit_buffer=0L;

    while (input_bit_count <= 24)
    {
        if ((c = getc_comp()) == -1)
            break;

        input_bit_buffer |=
                (unsigned long) c << (24-input_bit_count);
        input_bit_count += 8;
    }

    if(input_bit_count < CUR_BITS) {
        ATLASSERT(!input_bit_buffer);
        return -1; /* EOF */
    }

    return_value=input_bit_buffer >> (32-CUR_BITS);
    input_bit_buffer <<= CUR_BITS;
    input_bit_count -= CUR_BITS;

    ATLASSERT(return_value < (1UL << CUR_BITS));
    return(return_value);
}

unsigned char* Decoder::decode_string(unsigned char *buffer, unsigned int code) {
    int i;

    i=0;
    while (code >= FIRST_CODE)
    {
        *buffer++ = append_character[code];
        code=prefix_code[code];
        i++;
        ATLASSERT(i < sizeof(decode_stack)); /* buffer overrun if it blows, increase stack size! */
    }
    *buffer=code;
    return(buffer);
}

