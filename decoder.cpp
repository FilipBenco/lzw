#include "decoder.h"

Decoder::Decoder(string outputFile)
{
	try {
		this->lzw_file.open("output.lzw", ios::binary);
		if (this->lzw_file.is_open()) {
			
			
			this->output_file.open(outputFile.c_str(), ios::binary|ios::trunc);
			if (this->output_file.is_open()) {
				
				char c_check_code[4];
				this->lzw_file.get(c_check_code, 4);
				string check_code(c_check_code);
				std::cerr << check_code;
				if (check_code.compare("LZW") == 0 && this->lzw_file.get() == MIN_CODE_LEN) {
					this->expand();
				} else {
					throw new DecoderException("Subor'" + outputFile + "' nie je zakodovany kodovanim LZW");
				}
				
				
			} else {
				throw new DecoderException("Nepodarilo sa otvorit '" + outputFile + "' subor");
			}
			
			
		} else {
			throw new DecoderException("Nepodarilo sa otvorit 'output.lzw' subor");
		}
		
		this->lzw_file.close();
		this->output_file.close();
		
	} catch(exception &e) {
		std::cerr << e.what();
	}
}

void Decoder::expand() {
    unsigned int next_code;
    unsigned int new_code;
    unsigned int old_code;
    int character;
    unsigned char *string;
    unsigned int bit_limit;

    this->CUR_BITS = MIN_CODE_LEN;
    bit_limit = CURRENT_MAX_CODES(this->CUR_BITS) - 1;
    this->input_bit_count=0;
    this->input_bit_buffer=0L;


cerr << "expand: 55";

    next_code=FIRST_CODE;        /* This is the next available code to define */

    old_code=this->input_code();       /* Read in the first code, initialize the */
    
cerr << "expand: 61";
	character=old_code;          /* character variable, and send the first */
	// prepis putc_out funkcie
	this->output_file.put(old_code);
	if(!this->output_file.good()) {
		throw new DecoderException("Nepodarilo sa vlozit do vystupneho suboru inicializacny kod");
	}
	//
cerr << "expand: 69";
	
    /*
    **  This is the main expansion loop.  It reads in characters from the LZW file
    **  until it sees the special code used to inidicate the end of the data.
    */
	while(true) {
		try {
			new_code=this->input_code();
cerr << "expand: 78";
			
			/* look for code length increase marker */
			if(bit_limit == new_code && this->CUR_BITS < BITS) {
				this->CUR_BITS++;
				bit_limit = CURRENT_MAX_CODES(CUR_BITS) - 1;

				new_code=input_code();
			}
cerr << "expand: 87";

			/*
			** This code checks for the special STRING+CHARACTER+STRING+CHARACTER+STRING
			** case which generates an undefined code.  It handles it by decoding
			** the last code, and adding a single character to the end of the decode string.
			*/
			if (new_code>=next_code) {
cerr << "expand: 95";
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
cerr << "expand: 107: " << endl;
			
			character=*string;
			while (string >= decode_stack) {
				this->output_file.put(*string--);
				if(!this->output_file.good()) {
					throw new DecoderException("Nepodarilo sa vlozit do vystupneho suboru inicializacny kod");
				}
			}
			
			/*
			** Finally, if possible, add a new code to the string table.
			*/
			if (next_code <= MAX_CODE) {
				prefix_code[next_code]=old_code;
				append_character[next_code]=character;
				next_code++;
			}
			old_code=new_code;
cerr << "expand: 126";
			
		} catch (exception &e) {
			std::cerr << e.what();
			break;
		}
	}
    
}


int Decoder::input_code() {
	int c;
	
	while (this->input_bit_count <= 24)
	{
		
		// prepis funkcie getc_comp    
		c = this->lzw_file.get();
		if (!this->lzw_file.good())
			break;
		//
		
		this->input_bit_buffer |= (unsigned long) c << (24 - this->input_bit_count);
		this->input_bit_count += 8;
	}
	
	if(this->input_bit_count < this->CUR_BITS) {
		throw new DecoderException("Narazil som na koniec suboru");
	}
	
	unsigned int return_value = this->input_bit_buffer >> (32- this->CUR_BITS);
	this->input_bit_buffer <<= this->CUR_BITS;
	this->input_bit_count -= this->CUR_BITS;
	
	return return_value;
}


unsigned char* Decoder::decode_string(unsigned char *buffer, unsigned int code) {
	int i;
cerr << "decode_string: 168" << endl;
	i=0;
	while (code >= FIRST_CODE)
	{
cerr << "decode_string: 172" << endl;
		*buffer++ = append_character[code];
cerr << "decode_string: 174" << endl;
		code=prefix_code[code];
cerr << "decode_string: 176" << endl;
		i++;
	}
cerr << "decode_string: 179" << endl;
	*buffer=code;
cerr << "decode_string: 181" << endl;
	return buffer;
}

