#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include<unordered_map>
#include <algorithm>
#include <ranges>

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

	uint32_t operator()(size_t numbit) {
		uint32_t num = 0;
		while (numbit-- > 0) {
			num = (num<<1) | read_bit();
		}
		return num;
	}
};

template<typename T, typename CT=uint64_t> 
struct frequency {
	std::unordered_map<T,CT> counters_;

	void operator()(const T& x) {
		counters_[x]++;
	}

	size_t d_size() {
		return counters_.size();
	}

	uint32_t total() {
		uint32_t tot = 0;
		for (auto x : counters_) {
			tot +=x.second;
		}
		return tot;
	}

	auto begin() const { return counters_.begin(); }
	auto end() const { return counters_.end(); }
	auto size() const { return counters_.size(); }
};

struct node {
	char sym_;
	uint64_t prob_;
	node* left_ = nullptr, * right_ = nullptr;

	node(char sym, uint64_t prob) : sym_(sym), prob_(prob) {}
	node(node* a, node* b) : left_(a), right_(b), prob_(a->prob_ + b->prob_) {}
};

struct huffman {

	struct code {
		uint32_t len_, bit_;
	};
	std::unordered_map<char, code> code_table;

	void compute_codes(const node* n, uint32_t len, uint32_t bit) {
		if (n->left_ == nullptr) {
			code_table[n->sym_] = { len, bit };
		}
		else {
			compute_codes(n->left_, len+1, (bit << 1) | 0);
			compute_codes(n->right_, len+1, (bit << 1) | 1);
		}
	}
};

void encode(const std::string& filein, const std::string fileout) {
	std::ifstream is(filein, std::ios::binary);
	if (!is) {
		error("error in encode filein opening \n");
	}

	std::vector<uint8_t> v{ std::istream_iterator<char>(is), std::istream_iterator<char>() };
	frequency<char, uint64_t> fr;
	for_each(v.begin(), v.end(), [&fr](const uint8_t& x) { fr(x); });


	std::vector<std::unique_ptr<node>> storage;
	std::vector<node*> tree;

	for (auto x : fr.counters_) {
		node* n = new node (x.first, x.second);
		storage.emplace_back(n);
		tree.push_back(n);
	}
	
	std::sort(tree.begin(), tree.end(), [](const node* a, const node* b) { return a->prob_ > b->prob_; });

	while (tree.size() > 1) {
		node* a = tree.back();
		tree.pop_back();
		node* b = tree.back();
		tree.pop_back();
		node* c = new node(a, b);
		storage.emplace_back(c);
		auto it = tree.begin();
		for (; it < tree.end(); it++) {
			auto x = *it;
			//3
			// ++2
			//2
			//2
			//1 |
			//1	|
			if ((*it)->prob_ <= c->prob_)
				break;
		}
		tree.insert(it, c);
	}

	huffman hf;

	hf.compute_codes(tree.front(), 0, 0);
	

	std::ofstream os(fileout, std::ios::binary);
	if (!os) {
		error("error in encode fileout opening \n");
	}

	os << "HUFFMAN1";
	os.put(static_cast<char>(fr.d_size()));
	bit_writer bw(os);
	for (const auto& x : hf.code_table) {
		bw(x.first, 8);
		bw(x.second.len_, 5);
		bw(x.second.bit_, x.second.len_);
	}
	auto tot = fr.total();
	bw(tot, 32);

	is.clear();
	is.seekg(0);

	while (true) {
		int32_t x = is.get();
		if (x==EOF)
			break;
		bw(hf.code_table[x].bit_, hf.code_table[x].len_);
	}

}

//c esempio.txt out.bin
template<typename T>
std::istream& rawread(std::istream& is, T& magic, size_t size = sizeof(T)) {
	return is.read(reinterpret_cast<char*> (&magic), size);
}

void decode(const std::string& filein, const std::string fileout) {
	std::ifstream is(filein, std::ios::binary);
	if (!is) {
		error("error in encode filein opening \n");
	}

	std::string magic(8, ' ');
	is.read(magic.data(), 8);
	if (magic != "HUFFMAN1") {
		error("errore magic number " + magic);
	}

	uint16_t table_size = is.get();
	if (table_size == 0)
		table_size = 256;
	bit_reader br(is);
	struct triplet {
		uint64_t sym, bit;
		uint8_t len;

		bool operator<(triplet& rhs) {
			return len < rhs.len;
		}
	};
	std::vector<triplet> table;
	for (int i = 0; i < table_size; i++) {
		triplet t;
		t.sym = br(8);
		t.len = br(5);
		t.bit = br(t.len);
		table.emplace_back(t);
	}

	sort(table.begin(), table.end());

	std::ofstream os(fileout, std::ios::binary);
	if (!os) {
		error("error in encode fileout opening \n");
	}

	uint32_t tot_size = br(32);
	uint32_t pos = 0;
	for(int i=0; i<tot_size;++i){
		uint16_t j = 0;
		uint16_t len = 0;
		uint32_t code = 0;
		for (; j < table_size; j++) {
			if (len < table[j].len) {
				uint32_t curlen = table[j].len - len;
				code = (code<<curlen) | br(curlen);
				len = table[j].len;
			}
			if (code == table[j].bit) {
				os.put(table[j].sym);
				break;
			}
		}
		if (j == table_size)
			error("codice non presente in tabella");
	}
}

int main(int argc, char* argv[]) {
	
		if (argc != 4) {
			syntax();
		}

		std::string command = argv[1];
		if (command == "c") {
			encode(argv[2], argv[3]);
		}
		else if (command == "d") {
			decode(argv[2], argv[3]);
		}
		else {
			syntax();
		}
	_CrtDumpMemoryLeaks();

	return EXIT_SUCCESS;
}