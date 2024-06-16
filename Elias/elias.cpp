#include <fstream>
#include <iostream>
#include <vector>
#include <iterator>
#include <math.h>
#include <algorithm>
void error(const std::string& msg) {
	std::cout << msg << '\n';
	exit(EXIT_FAILURE);
}

void syntax() {
	error("Parametri non corretti;\n"
		"SYNTAX:\n"
		"elias [c|d] <filein> <fileout>\n");
}

class bit_writer {
	uint8_t buffer_ = 0;
	std::ostream& os_;
	size_t nbit_ = 0;
	void write_bit(uint64_t curbit) {
		buffer_ = (buffer_ << 1) + (curbit & 1);
		nbit_++;
		if (nbit_ == 8) {
			os_.put(buffer_);
			nbit_ = 0;
		}
	}
public:
	bit_writer(std::ostream& os) :os_(os) {};
	~bit_writer() {
		flush();
	}

	std::ostream& operator()(uint64_t num, uint64_t numbit) {
		
		while (numbit-- > 0) {
			write_bit(num>>numbit);
		}
		return os_;
	}

	std::ostream& flush() {
		while (nbit_ > 0)
			write_bit(0);
		return os_;
	}

};

class bit_reader {
	uint8_t buffer_=0;
	uint8_t nbit_=0;
	std::istream& is_;

	uint64_t read_bit() {
		if (nbit_ == 0) {
			buffer_ = is_.get();
			nbit_ = 8;
		}
		nbit_--;
		return (buffer_ >> nbit_) & 1;
	}
public:
	bit_reader(std::istream& is) : is_(is) {}

	uint64_t operator()(int32_t numbit) {
		uint64_t u = 0;
		while (numbit-- > 0) {
			u =(u << numbit) + read_bit();
		}
		return u;
	}
};

class elias_writer {
	uint64_t numbit=0;
	uint64_t map = 0;
	bit_writer bw_;
	uint64_t elias(int32_t num) {
		if (num >= 0) {
			map = 2 * num + 1;
		}
		else {
			map = -(2 * num);
		}
		return map;
	}
public:
	elias_writer(std::ostream& os) : bw_(os) {}
	
	void compress(int32_t num) {
		uint64_t mapped=elias(num);
		numbit = static_cast<uint64_t>(std::floor(log2(mapped))) * 2 + 1;
		std::cout << num << '\n';
		std::cout << mapped << '\n';
		std::cout<<numbit<<'\n';
		bw_(mapped, numbit);
	}	
};

class elias_reader {
	bit_reader br_;
	int32_t elias(uint64_t num) {
		if (num % 2 == 0) {
			return - static_cast<int32_t>((num) / 2);
		}

		return static_cast<int32_t>((num - 1) / 2);
	}
public:
	elias_reader(std::istream& is):br_(is){}

	int32_t decompress() {
		size_t cont = 0;
		while (br_(1) == 0) {
			cont++;
		}
		int32_t num = (1LL<<cont) + br_(cont);
		return elias(num);
	};

};

int main(int argc, char* argv[]) {
	if (argc != 4) {
		syntax();
	}
	std::string command = argv[1];

	if (command=="c") {
		std::ifstream is(argv[2]);
		if (!is) {
			error("cannot open filein\n");
		}
		
		std::ofstream os(argv[3], std::ios::binary);
		if (!os) {
			error("cannot open filout\n");
		}

		elias_writer ew(os);

		for (std::istream_iterator<int32_t> it(is); it != std::istream_iterator<int32_t>(); ++it)
			ew.compress(*it);


		//for_each(std::istream_iterator<int32_t>(is), std::istream_iterator<int32_t>(), [&](int32_t num) {ew.compress(num);});


	}else if (command == "d") {
		std::ifstream is(argv[2],std::ios::binary);
		if (!is) {
			error("cannot open filein\n");
		}

		std::ofstream os(argv[3]);
		if (!os) {
			error("cannot open filout\n");
		}

		elias_reader er(is);

		while (true) {
			int32_t num = er.decompress();
			if (!is) {
				break;
			}
			os << num << "\n";
		}
		
	}	else {
		syntax();
	}
	return EXIT_SUCCESS;
}