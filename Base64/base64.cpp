#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <cstdint>


void error(const std::string& msg) {
	std::cout << msg;
	exit(EXIT_FAILURE);
}

class bit_writer {
	uint8_t buffer_;
	std::ostream& os_;
	int n_;
	void writebit(uint64_t curbit) {
		n_++;
		buffer_ = (buffer_ << 1) | (curbit & 1);
		if (n_ == 8) {
			n_ = 0;
			os_.put(buffer_);
		}
	}
public:
	bit_writer(std::ostream& os) : os_(os), n_(0){}
	~bit_writer() {
		flush();
	}

	std::ostream& operator()(uint64_t num, int n_bit) {
		while (n_bit-- > 0) {
			writebit(num >> n_bit);
		}
		return os_;
	}

	std::ostream& flush(int pad=0) {
		while (n_ > 0)
			writebit(pad);
		return os_;
	}
};

std::string base64_decode(const std::string& input) {
	std::map<char, uint8_t> table;
	for (int i = 0; i < 62; ++i) {
		if (i < 26)
			table['A' + i] = i;
		else if (i < 52)
			table['a' + i - 26] = i;
		else
			table['0' + i - 52] = i;
	}
	table['+'] = 62;
	table['/'] = 63;
	std::ostringstream ss;
	bit_writer bw(ss);

	for (char c : input) {
		if (c == '=')
			break;
		auto it = table.find(c);
		if (it != table.end()) {
			bw(it->second, 6);
		}
	
	}
	return ss.str();
}

int main() {
	std::string s = base64_decode("c2hvcnQgdGV4dA==");
}