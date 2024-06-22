#include<cstdint>
#include<iostream>
#include<fstream>
#include"pgm.h"

void error(const std::string& msg) {
	std::cout << msg << '\n';
	exit(EXIT_FAILURE);
}

auto make_pattern() {
	mat<uint8_t> img(256, 256);

	for (int r = 0; r < img.rows(); ++r) {
		for (int c = 0; c < img.cols(); ++c) {
			img(r, c) = r;
		}
	}
	return img;
}

void flip(mat<uint8_t>& img) {
	for (int r = 0; r < img.rows(); ++r) {
		for (int c = 0; c < img.cols(); ++c) {
			std::swap(img(r, c), img(img.rows() - 1 - r, c));
		}
	}
}

auto read(const std::string& filename){

	mat<uint8_t> img;

	std::ifstream is(filename, std::ios::binary);
	if (!is) {
		return img;
	}
	std::string magic;

	std::getline(is, magic);
	pgm_mode mode;
	if (magic == "P5") {
		mode = pgm_mode::binary;
	}
	else if(magic=="P2") {
		mode = pgm_mode::plain;
	}
	else {
		error("errore magic number");
	}

	std::string comment;

	if (is.peek() == '#') {
		std::getline(is, comment);
	}
	
	int width=0, height=0, nlevel=0;
	char newline;

	is >> width, height, nlevel;
	is.get(newline);

	if (nlevel != 255 || newline != '\n') {
		return img;
	}

	img.resize(height, width);
	if (mode == pgm_mode::plain) {
		for (auto& x : img) {
			int value;
			is >> value;
			x = value;
		}
	}
	else {
		is.read(img.rawdata(), img.rawsize());
	}

	if (img.empty()) {
		error("empty img");
	}

	flip(img);
	write("frog_flipped.pam", img, pgm_mode::binary);


	return img;
}

int main(int argc, char* argv[]) {
	mat<uint8_t> img = make_pattern();

	write("test_plain.pgm", img, pgm_mode::plain);
	write("test_binary.pgm", img, pgm_mode::binary);


	img = read("frog.pam");

	


}