#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <cassert>
#include <cstdint>
#include <array>
#include <algorithm>

#include "mat.h"

void error(const std::string& msg) {
	std::cout << msg << '\n';
	exit(EXIT_FAILURE);
}

bool save(const mat<uint8_t>& img, const std::string& filename) {
	std::ofstream os(filename, std::ios::binary);
	if (!os) {
		return false;
	}

	os << "P7\n";
	os << "WIDTH " << img.cols() << "\n";
	os << "HEIGHT " << img.rows() << "\n";
	os << "DEPTH 3\n";
	os << "MAXVAL 255\n";
	os << "TUPLTYPE RGB\n";
	os << "ENDHDR\n";

	for (int r = 0; r < img.rows(); r++) {
		for (int c = 0; c < img.cols(); c++) {
			os.put(img(r, c));
		}
	}
	return true;
}


using rgb = std::array<uint8_t, 3>;
bool save(const mat<rgb>& img, const std::string& filename) {
	std::ofstream os(filename, std::ios::binary);
	if (!os) {
		return false;
	}

	os << "P7\n";
	os << "WIDTH " << img.cols() << "\n";
	os << "HEIGHT " << img.rows() << "\n";
	os << "DEPTH 3\n";
	os << "MAXVAL 255\n";
	os << "TUPLTYPE RGB\n";
	os << "ENDHDR\n";

	for (int r = 0; r < img.rows(); r++) {
		for (int c = 0; c < img.cols(); c++) {
			os.put(img(r, c)[0]);
			os.put(img(r, c)[1]);
			os.put(img(r, c)[2]);
		}
	}
	return true;
}

mat<rgb>& load(mat<rgb>& img,const std::string& filename) {
	std::ifstream is(filename, std::ios::binary);
	if (!is) {
		error("errore caricamento " + filename + '\n');
	}
	std::string magic;
	
	std::getline(is, magic);
	if (magic != "P7") {
		error("wrong magic number");
	}

	int h, w;

	while (1) {
		std::string line;
		is >> line;
		if (line == "ENDHDR") {
			break;
		}

		if (line == "WIDTH") {
			is >> w;
		}

		if (line == "HEIGHT") {
			is >> h;
		}

		if (line == "DEPHT") {
			int d;
			is >> d;
			if (d != 3) {
				error("error in depht");
			}
		}

		if (line == "MAXVAL") {
			int maxval;
			is >> maxval;
			if (maxval != 255) {
				error("error maxval");
			}
		}

		if (line == "TUPLTYPE") {
			std::string s;
			is >> s;
			if (s != "RGB") {
				error("not RGB");
			}
		}

	}
	img.resize(h, w);

	for (int r = 0; r < img.rows(); r++) {
		for (int c = 0; c < img.cols(); c++) {
			img(r, c)[0] = is.get();
			img(r, c)[1] = is.get();
			img(r, c)[2] = is.get();
		}
	}
	return img;
}

void mirror(mat<rgb>& img) {
	for (int r = 0; r < img.rows(); r++) {
		for (int c = 0; c < img.cols(); c++) {
			std::swap(img(r, img.cols() - c - 1)[0], img(r, c)[0]);
			std::swap(img(r, img.cols() - c - 1)[1], img(r, c)[1]);
			std::swap(img(r, img.cols() - c - 1)[2], img(r, c)[2]);
		}
	}


}

int main() {
	mat<rgb> img;
	load(img, "laptop.pam");
	mirror(img);
	save(img, "reddd.pam");
	return 0;
}