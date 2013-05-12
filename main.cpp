#include "encoder.h"
#include "decoder.h"
#include <iostream>

void program_use(char *program_name) {
		std::cerr << "Program use: " << std::endl;
		std::cerr << std::endl;
		std::cerr << "Encode the input file and exporting it to lzw file format:" << std::endl;
		std::cerr << "\"" << program_name << " input_file.bmp output_file.lzw\"" << std::endl;
		std::cerr << std::endl;
		std::cerr << "Decode encoded lzw file format spaecified by \"ipnut_file.lzw\" into output file:" << std::endl;
		std::cerr << "\"" << program_name << " --decode input_file.lzw output_file.bmp\"" << std::endl;
		std::cerr << "\"" << program_name << " -d input_file.lzw output_file.bmp\"" << std::endl;
		std::cerr << std::endl;
}

int main(int argc, char *argv[]) {
	
	if (argc >= 3) {
		string arg1(argv[1]);
		string arg2(argv[2]);
		if (arg1.compare("--decode")==0 || arg1.compare("-d")==0) {
			if (argc == 4) {
				string arg3(argv[3]);
				
				Decoder decoder(arg2, arg3);
			} else {
				program_use(argv[0]);
				return -1;
			}
		} else {
			Encoder encoder(arg1, arg2);
		}
	} else {
		program_use(argv[0]);
		return -1;
	}
	
	return 0;
}
