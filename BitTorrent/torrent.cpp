#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>
#include <string>

void error(const std::string& msg) {
	std::cerr << msg << std::endl;
	exit(EXIT_FAILURE);
}



void torrent_dump(std::istream& is, std::vector<std::string>& tor, std::string& tmp) {

	char x = is.peek();
	int64_t integ;
	if (!is)
		return;
	if (x == 'd') {
		is.get();
		std::cout << '{'<< "\n ";
		tor.push_back("dic");
		while (is.peek() != 'e') {
			torrent_dump(is, tor, tmp);
			std::cout << "=>";
		}
		std::cout  << "\n " << '}';
	}
	else if (x == 'l') {
		is.get();
		std::cout << '['<<'\n';
		tor.push_back("lis");
		while (is.peek() != 'e') {
			torrent_dump(is, tor, tmp);
			std::cout << "=>";
		}
	}
	else if (x == 'i') {
		is.get();
		int64_t integ;
		is >> integ;
		std::cout << integ << " ";
		if (is.get() != 'e') 
			error("errore in integer");		
	}
	else {
		uint64_t numbyte;
		is >> numbyte;
		if (is.get() != ':') 
			error("errore in string");

		std::string str;
		if (numbyte > 20 && (numbyte % 20 == 0))
			for (int i = 0; i < numbyte / 20; ++i) {
				str.resize(20);
				is.read(&str[0], 20);
				std::cout << str << "\n";
			}
		else {
			str.resize(numbyte);
			is.read(&str[0], numbyte);
			std::cout << str;
		}

	}
	
	
}

int main(int argc, char* argv[]) {
	std::ifstream is(argv[1], std::ios::binary);
	if (!is)
		error("errore apertura filein");
	std::vector<std::string> vec;
	std::string tmp="";
	torrent_dump(is, vec, tmp);
}