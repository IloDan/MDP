#include<cstdint>
#include<iostream>
#include<fstream>
#include<string>


void error(const std::string& msg) {
	std::cerr << msg <<std::endl;
	exit(EXIT_FAILURE);
}

void run(std::ostream& os, char c, int& run_c) {
	uint16_t bit = 257 - (run_c + 1);
	os.put(bit);
	os.put(c);
	run_c = 0;
}

void copy(std::ostream& os, std::string& copy_str, int& copy_c) {
	os.put(copy_c - 1);
	for (char p : copy_str)
		os.put(p);
	copy_c = 0;
	copy_str = "";
}

void encode(std::string filein, std::string fileout) {
	std::ifstream is(filein, std::ios::binary);
	if (!is) {
		error("errore encoder apertura filein: " + filein);
	}
	std::ofstream os(fileout, std::ios::binary);
	if (!os) {
		error("errore encoder apertura filein: " + filein);
	}

	int copy_c = 0;
	int run_c = 0;
	std::string copy_str;
	while (true) {
		char c = is.get();		
		if (is.peek() == c) {
			if (run_c == 127) 
				run(os, c, run_c);
			else {
				run_c++;
				if (copy_c > 0)
					copy(os, copy_str, copy_c);
			}
		}
		else {
			if (run_c > 0) 
				run(os, c, run_c);
			else {			
				copy_c++;
				copy_str.push_back(c);
				if (copy_c == 128)
					copy(os, copy_str, copy_c);		
			}
		}
		if (!is) {
			os.put(128);
			break;
		}
	}
}

void encode_variant(std::string filein, std::string fileout) {
	std::ifstream is(filein, std::ios::binary);
	if (!is) {
		error("errore encoder apertura filein: " + filein);
	}
	std::ofstream os(fileout, std::ios::binary);
	if (!os) {
		error("errore encoder apertura filein: " + filein);
	}

	int copy_c = 0;
	int run_c = 0;
	std::string copy_str;
	while (true) {
		char c = is.get();
		if (is.get() == c) {
			if (run_c == 127)
				run(os, c, run_c);
			else {

				if (copy_c > 0) {
					if (is.peek() == c){
						copy(os, copy_str, copy_c);
						run_c++;
					}else {
						copy_c++;
						copy_str.push_back(c);
					}

				}else
					run_c++;
							
			}
		}
		else {
			if (run_c > 0)
				run(os, c, run_c);
			else {
				copy_c++;
				copy_str.push_back(c);
				if (copy_c == 128)
					copy(os, copy_str, copy_c);
			}
		}
		is.unget();
		if (!is) {
			if(copy_c!=0)
				copy(os, copy_str, copy_c);
			os.put(128);
			break;
		}
	}
}

void decode(std::string filein, std::string fileout) {
	std::ifstream is(filein, std::ios::binary);
	if (!is) {
		error("errore encoder apertura filein: " + filein);
	}
	std::ofstream os(fileout, std::ios::binary);
	if (!os) {
		error("errore encoder apertura filein: " + filein);
	}

	while (true) {
		uint8_t r_c=is.get();
		if (r_c == 128)
			break;
		if (!is)
			error("no 128 terminator encounter before eof");
		
		if (r_c < 128) {
			int copy = r_c + 1;
			while (copy > 0) {
				char c = is.get();
				os.put(c);
				copy--;
			}
		}
		else {
			char c = is.get();
			int run = 257 - r_c;
			while (run > 0) {
				os.put(c);
				run--;
			}
		}
	}


}

int main(int argc, char* argv[]) {

	if (argc != 4) {
		error("wrong number of argument " + std::to_string(argc) + " <4 ");
	}
	std::string command = argv[1];
	if (command == "c") 
		encode_variant(argv[2], argv[3]);
	else
		decode(argv[2], argv[3]);


}