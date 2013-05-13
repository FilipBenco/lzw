#include "decoder.h"
#include <iostream>

Decoder::Decoder(string inputFile, string outputFile)
{
	try {
		this->lzw_file.open(inputFile.c_str(), ios::binary);
		if (this->lzw_file.is_open()) {
			
			
			this->output_file.open(outputFile.c_str(), ios::binary|ios::trunc);
			if (this->output_file.is_open()) {
				
				char c_check_code[ sizeof(LZW_DETECTION) +1 ];
				this->lzw_file.get(c_check_code, sizeof(c_check_code));
				string check_code(c_check_code);
				
				if (check_code.compare(LZW_DETECTION) == 0 && this->lzw_file.get() == MIN_CODE_LEN) {
					this->expand();
				} else {
					throw "File '" + inputFile + "' has not LZW format";
				}
				
				
			} else {
				throw "Unable to open file '" + outputFile + "'";
			}
			
			
		} else {
			throw "Unable to open file '" + inputFile + "'";
		}
		
		this->lzw_file.close();
		this->output_file.close();
		
	} catch(string str) {
		std::cerr << str << std::endl;
	} catch(char* str) {
		std::cerr << str << std::endl;
	}

}

void Decoder::expand() {
    unsigned int next_code, new_code, old_code, bit_limit;
    int character;
    unsigned char *string;


    this->current_bits = MIN_CODE_LEN;
    bit_limit = CURRENT_MAX_CODES(this->current_bits) - 1;
    this->input_bit_count=0;
    this->input_bit_buffer=0L;



    next_code=FIRST_CODE; 
    old_code=this->input_code();
	character=old_code;


	this->output_file.put(old_code);
	if(!this->output_file.good()) {
		throw "Unable to insert initialisation code into output file";
	}
	

	while(true) {
		try {
			new_code=this->input_code();

			if(bit_limit == new_code && this->current_bits < BITS) {
				this->current_bits++;
				bit_limit = CURRENT_MAX_CODES(current_bits) - 1;

				new_code=input_code();
			}


			if (new_code>=next_code) {
				*decode_stack=character;
				string=decode_string(decode_stack+1,old_code);
			} else {
				string=decode_string(decode_stack,new_code);
			}


			character=*string;
			while (string >= decode_stack) {
				this->output_file.put(*string--);
				if(!this->output_file.good()) {
					throw "Unable to insert decoded code into output file";
				}
			}
			
			

			if (next_code <= MAX_CODE) {
				prefix_code[next_code]=old_code;
				append_character[next_code]=character;
				next_code++;
			}
			old_code=new_code;
			

		} catch(std::string str) {
			std::cerr << str << std::endl;
			break;
		} catch(char* str) {
			std::cerr << str << std::endl;
			break;
		}
	}
    
}


int Decoder::input_code() {
	int c;
	
	while (this->input_bit_count <= 24) {
		  
		c = this->lzw_file.get();
		if (!this->lzw_file.good())
			break;
		
		this->input_bit_buffer |= (unsigned long) c << (24 - this->input_bit_count);
		this->input_bit_count += 8;
	}
	
	if(this->input_bit_count < this->current_bits) {
		throw "Reached end of input file";
	}
	
	unsigned int return_value = this->input_bit_buffer >> (32- this->current_bits);
	this->input_bit_buffer <<= this->current_bits;
	this->input_bit_count -= this->current_bits;
	
	return return_value;
}


unsigned char* Decoder::decode_string(unsigned char *buffer, unsigned int code) {
	int i=0;
	while (code >= FIRST_CODE) {
		*buffer++ = append_character[code];
		code=prefix_code[code];
		i++;
	}
	*buffer=code;
	return buffer;
}

