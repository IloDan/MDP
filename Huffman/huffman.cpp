#include<iostream>
#include<string>
#include<fstream>

void error(const std::string& msg) {
	std::cout << msg << '\n';
	exit(EXIT_FAILURE);
}

void syntax() {
	error("SYNTAX \n"
		"huffman [c|d] <filein.txt/bin> <fileout.txt/bin>\n");
}

class bit_writer {
	uint8_t buffer_;
	size_t n_ = 0;
	std::ostream& os_;
	void write_bit(uint64_t bit) {
		buffer_ = (buffer_ << 1) | (bit & 1);
		++n_;
		if (n_ == 8) {
			os_.put(buffer_);
			n_ = 0;
		}
	}
public:
	bit_writer(std::ostream& os):os_(os){}
	~bit_writer() {
		flush();
	}

	std::ostream& operator()(uint64_t x, size_t numbit) {
		while (numbit-- > 0) {
			write_bit(x >> numbit);
		}
		return os_;
	}

	std::ostream& flush(int pad = 0) {
		while (n_ > 0) {
			write_bit(pad);
		}
		return os_;
	}
};

class bit_reader {
	uint8_t buffer_;
	size_t n_ = 0;
	std::istream& is_;

	uint64_t read_bit() {
		if (n_ == 0) {
			buffer_=is_.get();
			n_ = 8;
		}
		--n_;
		return (buffer_ >> n_) & 1;
	}
public:
	bit_reader(std::istream& is): is_(is){}

	uint64_t operator()(size_t numbit) {
		uint64_t num = 0;
		while (numbit-- > 0) {
			num = num<<1 | read_bit();
		}
	}
};


