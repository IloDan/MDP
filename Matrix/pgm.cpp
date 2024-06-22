#include<fstream>
#include<string>
#include "matrix.h"

enum class pgm_mode { plain, binary };
bool write(const std::string& filename, mat<uint8_t>& img, pgm_mode mode) {
	std::ofstream os(filename, std::ios::binary);
	if (!os) {
		return false;
	}

	if (mode == pgm_mode::plain) {
		os << "P2\n";
	}
	else {
		os << "P5\n";
	}

	os << "#MDP ma dio porco\n";
	os << img.cols() << " " << img.rows() << "\n255\n";

	if (mode == pgm_mode::plain) {
		for (int r = 0; r < img.rows(); ++r) {
			for (int c = 0; c < img.cols(); ++c) {
				os << int(img(r, c)) << ' ';
			}
			os << '\n';
		}
	}

	if (mode == pgm_mode::binary) {
		os.write(img.rawdata(), img.rawsize());
	}

	return true;
}

