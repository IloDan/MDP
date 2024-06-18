#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include<unordered_map>
#include <algorithm>

void error(const std::string& msg) {
	std::cout << msg << '\n';
	exit(EXIT_FAILURE);
}

void syntax() {
	error("SYNTAX \n"
		"huffman [c|d] <filein.bin> <fileout.bin>\n");
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

template<typename T, typename CT=uint64_t> 
struct frequency {
	std::unordered_map<T,CT> counters_;

	void operator()(const T& x) {
		counters_[x]++;
	}

	auto begin() const { return counters_.begin(); }
	auto end() const { return counters_.end(); }
	auto size() const { return counters_.size(); }
};


void encode(const std::string& filein, const std::string fileout) {
	std::ifstream is(filein, std::ios::binary);
	if (!is) {
		error("error in encode filein opening \n");
	}

	std::vector<uint8_t> v{ std::istream_iterator<char>(is), std::istream_iterator<char>() };
	frequency<uint8_t, uint64_t> fr;
	for_each(v.begin(), v.end(), [&fr](const uint8_t& x){ fr(x); });

	struct node {
		char sym_;
		uint64_t prob_;
		node* left_ = nullptr, * right_ = nullptr;

		node(char sym, uint64_t prob): sym_(sym), prob_(prob){}
	};

	std::vector<node*> vec;

	for (auto x : fr.counters_) {
		node *n = new node*(x.first,x.second)
	}


}

void decode(const std::string& filein, const std::string fileout) {

}

int main(int argc, char* argv[]) {
	
	if (argc != 4) {
		syntax();
	}

	std::string command = argv[1];
	if (command == "c") {
		encode(argv[2],argv[3]);
	}else if (command=="d") {
		decode(argv[2], argv[3]);
	}
	else {
		syntax();
	}


	return EXIT_SUCCESS;
}