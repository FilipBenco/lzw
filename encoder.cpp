#include "encoder.h"
#include <iostream>

Encoder::Encoder(string inputFile, string outputFile) {
	//initializa properties
	this->code_value=(int*)malloc(TABLE_SIZE*sizeof(int));

	//run encoding
	try {
		this->input_file.open(inputFile.c_str(), ios::binary);
		if (this->input_file.is_open()) {

			this->lzw_file.open(outputFile.c_str(), ios::binary|ios::trunc);
			if(this->lzw_file.is_open()) {
				//writting detection string and min code length
				this->lzw_file.write(LZW_DETECTION, sizeof(LZW_DETECTION));
				this->lzw_file.put(MIN_CODE_LEN);

				this->encode();  //actual encoding

				this->lzw_file.close();

			} else {
				throw "Could not create lzw file: " + outputFile;
			}

			this->input_file.close();
		} else {
			throw "Could not read original file: " + inputFile;
		}
	} catch(string ex) {
		std::cerr << ex << std::endl;
	} catch(char* ex) {
		std::cerr << ex << std::endl;
	}
}

Encoder::~Encoder() {
	//cleanup
	free(this->code_value);
}

void Encoder::encode() {
    unsigned int next_code, character, string_code, index, bit_limit;

    this->current_bits = MIN_CODE_LEN;
    bit_limit = CURRENT_MAX_CODES(this->current_bits) - 1;
    this->output_bits_count=0;
    this->output_bits_buffer=0L;

    next_code=FIRST_CODE;  
    for (int i=0; i<TABLE_SIZE; i++)
        code_value[i]=-1;

    string_code = this->input_file.get();
    if(!this->input_file.good())
		throw "Input file is empty or corrupted."; 

    while (this->input_file.good()) {

        character=this->input_file.get();

        if(this->input_file.good()) {
            index = this->find_code_index(string_code,character);
            if (this->code_value[index] != -1) {  //we check if we have current string in our code table    
                string_code=this->code_value[index];
			} else {  
                if (next_code <= MAX_CODE)  {
                    this->code_value[index]=next_code++;
                    this->prefix_code[index]=string_code;
                    this->append_character[index]=character;
                }
                if(string_code >= bit_limit && this->current_bits < BITS) {
                    this->output_code(bit_limit);
                    this->current_bits++;
                    bit_limit = (CURRENT_MAX_CODES(this->current_bits) - 1);
                }

                output_code(string_code); 
                string_code=character;      
            }                              
        }
    }

    this->output_code(string_code); 
    if(this->output_bits_count) { //writes out last buffered bits
        this->output_bits_buffer >>= 24;
        this->lzw_file.put(output_bits_buffer);
        this->output_bits_count = 0;
        this->output_bits_buffer = 0;
    }
}

int Encoder::find_code_index(unsigned int hash_prefix, unsigned int hash_character) {
    int index;
    int offset;

	//append prefix to the character
    index = (hash_character << HASHING_SHIFT) ^ hash_prefix;
    if (index == 0)
        offset = 1;
    else
        offset = TABLE_SIZE - index;
    while (true) {
        if (this->code_value[index] == -1)
            return(index);
        if (this->prefix_code[index] == hash_prefix &&
                this->append_character[index] == hash_character)
            return(index);
        index -= offset;
        if (index < 0)
            index += TABLE_SIZE;
    }
}

void Encoder::output_code(unsigned int code) {
    this->output_bits_buffer |= (unsigned long) code << (32 - this->current_bits - output_bits_count);
    this->output_bits_count += this->current_bits;
    while (this->output_bits_count >= 8) {

        this->lzw_file.put(output_bits_buffer >> 24);
        this->output_bits_buffer <<= 8;
        this->output_bits_count -= 8;

    }
}


