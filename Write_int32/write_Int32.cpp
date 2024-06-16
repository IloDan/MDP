#include<cstdint>
#include<iostream>
#include<fstream>
#include<string>
#include<iterator>
#include<vector>

void error(const std::string& msg) {
	std::cout << msg << '\n';
	exit(EXIT_FAILURE);
}

void syntax() {
	error(
		"Syntax: \n"
		" write_int32 <filein.txt> <fileout.bin>\n"
	);
}

template <typename T>
void check_open(const T& stream, const std::string& filename) {
	if (!stream) {
		error("Error: Cannot open file " + filename);
	}
}
template <typename T>
std::ostream& rawWrite(std::ostream& os, const T& val) {
	return os.write(reinterpret_cast<const char*>(&val), sizeof(T));
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		syntax();
	}

	std::ifstream is_(argv[1]);
	check_open(is_, argv[1]);

	std::vector<int32_t> v{ std::istream_iterator<int32_t>(is_), std::istream_iterator<int32_t>() };

	std::ofstream os_(argv[2], std::ios::binary);
	check_open(os_, argv[2]);

	for (const auto& val : v) {
		rawWrite(os_, val);
	}

	return EXIT_SUCCESS;
}