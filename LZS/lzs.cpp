#include<fstream>
#include<cstdint>
#include<string>
#include<vector>

#include"lzs.h"

void error(const std::string& msg) {
	std::cerr << msg << std::endl;
	exit(EXIT_FAILURE);
}

class bit_reader {
	uint8_t buffer_;
	int n_;
	std::istream& is_;
	uint8_t read_bit() {
		if (n_ == 0) {
			buffer_ = is_.get();
			n_ = 8;
		}
		n_--;
		return (buffer_ >> n_) & 1;
	}
public:
	bit_reader(std::istream& is) : is_(is), n_(0) {}

	uint64_t operator()(int n_bit) {
		int64_t x = 0;
		while (n_bit-- > 0) {
			x = (x << 1) | read_bit();
		}
		return x;
	}
};

void lzs_decompress(std::istream& is, std::ostream& os) {
	
	std::vector<char> dic;
	bit_reader br(is);

	while (true) {
		uint8_t type = br(1);
		if (type == 0) {
			dic.push_back(is.get());
		}
		else {
			uint16_t off = br(1);
			if (off == 1)
				off = br(7);
			else
				off = br(11);

			uint16_t len = br(2);
			if (len == 3) {
				len = (len << 2) | br(2);
				if (len == 15) {
					uint16_t count = 1;
					while (true) {
						len = br(4);
						if (len != 15)
							break;
						count++;
					}
					len = count * 15 - 7;
				}
			}
		}
	}
	
}

int main() {

	std::ifstream is("nome", std::ios::binary);
	if (!is)
		error("errore apertura filein");

	std::ofstream os("nome", std::ios::binary);
	if (!is)
		error("errore apertura fileout");

	lzs_decompress(is, os);

	return EXIT_SUCCESS;
}