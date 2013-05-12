#include "encoder.h"

Encoder::Encoder(string input)
{
    this->input_file.open(input.c_str(),ios::binary);
    if (input_file.is_open()) {
        this->lzw_file.open("output.lzw",ios::binary|ios::trunc);
        if(lzw_file.is_open()) {
	    lzw_file.write("LZW", 3);
            lzw_file.put(MIN_CODE_LEN);
            this->encode();
            lzw_file.close();
        }
        input_file.close();
    }
}

void Encoder::encode() {
    unsigned int next_code;
    unsigned int character;
    unsigned int string_code;
    unsigned int index;
    unsigned int bit_limit;

    CUR_BITS = MIN_CODE_LEN;
    bit_limit = CURRENT_MAX_CODES(CUR_BITS) - 1;
    output_bits_count=0;
    output_bits_buffer=0L;

    next_code=FIRST_CODE;       /* Next code is the next available string code*/
    for (int i=0; i<TABLE_SIZE; i++) /* Clear out the string table before starting */
        code_value[i]=-1;

    /* Get the first code                         */
    string_code = input_file.get();
    if(!input_file.good())
        return; /* empty file or error */

    /*
    ** This is the main loop where it all happens.  This loop runs util all of
    ** the input has been exhausted.  Note that it stops adding codes to the
    ** table after all of the possible codes have been defined.
    */
    while (input_file.good())
    {
        character=input_file.get();
        if(input_file.good()) {
            index=find_match(string_code,character);/* See if the string is in */
            if (code_value[index] != -1)            /* the table.  If it is,   */
                string_code=code_value[index];        /* get the code value.  If */
            else                                    /* the string is not in the*/
            {   /* table, try to add it.   */
                if (next_code <= MAX_CODE)
                {
                    code_value[index]=next_code++;
                    prefix_code[index]=string_code;
                    append_character[index]=character;
                }

                /* are we using enough bits to write out this code word? */
                if(string_code >= bit_limit && CUR_BITS < BITS)
                {
                    /* mark need for bigger code word with all ones */
                    output_code(bit_limit);
                    CUR_BITS++;
                    bit_limit = (CURRENT_MAX_CODES(CUR_BITS) - 1);
                }

                output_code(string_code);  /* When a string is found  */
                string_code=character;            /* that is not in the table*/
            }                                   /* I output the last string*/
        }                                     /* after adding the new one*/
    }
    /*
    ** End of the main loop.
    */

    output_code(string_code); /* Output the last code               */
    output_code(-1);
}

int Encoder::find_match(unsigned int hash_prefix, unsigned int hash_character) {
    int index;
    int offset;

    index = (hash_character << HASHING_SHIFT) ^ hash_prefix;
    if (index == 0)
        offset = 1;
    else
        offset = TABLE_SIZE - index;
    while (1)
    {
        if (code_value[index] == -1)
            return(index);
        if (prefix_code[index] == hash_prefix &&
                append_character[index] == hash_character)
            return(index);
        index -= offset;
        if (index < 0)
            index += TABLE_SIZE;
    }
}

void Encoder::output_code(unsigned int code) {
    //static int output_bit_count=0;
    //static unsigned long output_bit_buffer=0L;

    if(EOF == code) {
        /* pad remaining zeros and flush the last byte */
        if(output_bits_count) {
            output_bits_buffer >>= 24;
            lzw_file.put(output_bits_buffer);
            output_bits_count = 0;
            output_bits_buffer = 0; /* in case some eejit calls us again */
        }
        return;
    }

    /* sends new bytes near the top (MSB) */
    output_bits_buffer |= (unsigned long) code << (32-CUR_BITS-output_bits_count);
    output_bits_count += CUR_BITS;
    while (output_bits_count >= 8)
    {
        /* no check for error but if there was a problem we'd know from the time we wrote the identifier */
        lzw_file.put(output_bits_buffer >> 24);
        output_bits_buffer <<= 8;
        output_bits_count -= 8;
    }
}


