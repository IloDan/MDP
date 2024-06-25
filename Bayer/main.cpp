#include <iostream>
#include <vector>
#include <cstdint>
#include <fstream>
#include <string>
#include <array>
#include <cmath>

#include "mat.h"


void error(const std::string& msg) {
	std::cerr << msg << std::endl;
	exit(EXIT_FAILURE);
}


bool load_pgm16(const std::string& filename, mat<uint16_t>& img) {
	std::ifstream is(filename, std::ios::binary);
	if (!is) {
		error("Error opening file " + filename);
	}
	std::string magic;
	is >> magic;
	if (magic != "P5") {
		error("Error: file is not a PGM file");
	}
	uint32_t width, height, maxval;
	is >> width >> height >> maxval;
	if (maxval != 65535) {
		error("Error: maxval is not 255");
	}
	img.resize(height, width);
	is.read(img.rawData(), img.rawsize());
	return true;
}


bool save_pgm(const std::string& filename, const mat<uint8_t>& img) {
	std::ofstream os(filename, std::ios::binary);
	if (!os) {
		error("Error opening file " + filename);
	}
	os << "P5\n" << img.cols() << " " << img.rows() << "\n" << 255 << "\n";
	os.write(img.rawData(), img.rawsize());
	return true;
	}


using rgb = std::array<uint8_t, 3>;
bool load_ppm(const std::string& filename, mat<rgb>& img) {
	std::ifstream is(filename, std::ios::binary);
	if (!is) {
		error("Error opening file " + filename);
	}
	std::string magic;
	is >> magic;
	if (magic != "P6") {
		error("Error: file is not a PGM file");
	}
	uint32_t width, height, maxval;
	is >> width >> height >> maxval;
	if (maxval != 65535) {
		error("Error: maxval is not 255");
	}
	img.resize(height, width);
	is.read(img.rawData(), img.rawsize());
	return true;
}

bool save_ppm(const std::string& filename, const mat<rgb>& img) {
	std::ofstream os(filename, std::ios::binary);
	if (!os) {
		error("Error opening file " + filename);
	}
	os << "P6\n" << img.cols() << " " << img.rows() << "\n" << 255 << "\n";
	os.write(img.rawData(), img.rawsize());
	return true;
}

void from16to8(const mat<uint16_t>& img, mat<uint8_t>& img8) {
	img8.resize(img.rows(), img.cols());
	//from 16 to 8 bits division by 256
	for (int i = 0; i < img.size(); ++i) {
		img8[i] = img[i] / 256;
	}
}
const int R = 0;
const int G = 1;
const int B = 2;

void to_rgb(const mat<uint8_t>& img, mat<rgb>& img_rgb) {
	img_rgb.resize(img.rows(), img.cols());
	for(int r=0; r<img.rows(); ++r){
		for (int c = 0; c < img.cols(); ++c)
			if (r % 2 == 0 && c % 2 == 0)
				img_rgb(r, c)[R] = img(r, c);
			else if ((r % 2 == 0 && c % 2 != 0) || (r % 2 != 0 && c % 2 == 0))
				img_rgb(r,c)[G] = img(r, c);
			else 
				img_rgb(r, c)[B] = img(r, c);
	}
}

uint8_t clamp(int col, int min =0, int max=255) {
	if (col < 0)
		return 0;
	else if (col > 255)
		return 255;
	return static_cast<uint8_t>(col);
}

void green_interpolation(mat<rgb>& img) {
	for (int r = 0; r < img.rows(); ++r) {
		for (int c = 0; c < img.cols(); ++c)
			if ((r % 2 == 0 && c % 2 == 0)|| (r % 2 != 0 && c % 2 != 0)){
				
				int X;
				if (r % 2 == 0 && c % 2 == 0) X = R;
				else X = B;

				int H = abs((img(r, c - 1)[G] - img(r, c + 1)[G])) + abs((img(r, c)[X] - img(r, c - 2)[X] + img(r, c)[X] - img(r, c + 2)[X]));
				int V = abs((img(r - 1, c)[G] - img(r + 1, c)[G])) + abs((img(r, c)[X] - img(r - 2, c)[X] + img(r, c)[X] - img(r + 2, c)[X]));
				
				int green;
				if (H < V) 
					green = ((img(r, c - 1)[G] + img(r, c + 1)[G]) / 2) + ((img(r, c)[X] - img(r, c - 2)[X] + img(r, c)[X] - img(r, c + 2)[X]) / 4);
				else if (H > V)
					green = (((img(r - 1, c)[G] + img(r + 1, c)[G])) / 2) + ((img(r, c)[X] - img(r - 2, c)[X] + img(r, c)[X] - img(r + 2, c)[X]) / 4);
				else 
					green = ((img(r, c - 1)[G] + img(r, c + 1)[G] + img(r - 1, c)[G] + img(r + 1, c)[G]) / 4) - 
								((img(r, c)[X] - img(r - 2, c)[X] + (img(r, c)[X] - img(r, c - 2)[X] + 
									img(r, c)[X] - img(r, c + 2)[X]) + img(r, c)[X] - img(r + 2, c)[X]) / 8);
				img(r, c)[G] = clamp(green);
			}
	}
}

void RB_interpolation(mat<rgb>& img) {
	for (int r = 0; r < img.rows(); ++r) {
		for (int c = 0; c < img.cols(); ++c) {
			
			if ((r % 2 == 0 && c % 2 == 0) || (r % 2 != 0 && c % 2 != 0)) {
				int X;
				if (r % 2 == 0 && c % 2 == 0) X = R;
				else X = B;

				int N = abs(img(r - 1, c - 1)[X] - img(r + 1, c + 1)[X]) + abs(img(r, c)[G] - img(r - 1, c - 1)[G] + img(r, c)[G] - img(r + 1, c + 1)[G]);
				int P = abs(img(r - 1, c + 1)[X] - img(r + 1, c - 1)[X]) + abs(img(r, c)[G] - img(r - 1, c + 1)[G] + img(r, c)[G] - img(r + 1, c - 1)[G]);

				int col;
				if (N < P)
					col = (img(r - 1, c - 1)[X] + img(r + 1, c + 1)[X]) / 2 + (img(r, c)[G] - img(r - 1, c - 1)[G] + img(r, c)[G] - img(r + 1, c + 1)[G]) / 4;
				else if (N > P)
					col = (img(r - 1, c + 1)[X] + img(r + 1, c - 1)[X]) / 2 + (img(r, c)[G] - img(r - 1, c + 1)[G] + img(r, c)[G] - img(r + 1, c - 1)[G]) / 4;
				else
					col = (img(r - 1, c - 1)[X] + img(r + 1, c + 1)[X] + img(r - 1, c + 1)[X] + img(r + 1, c - 1)[X]) / 4 +
						(img(r, c)[G] - img(r - 1, c - 1)[G] + img(r, c)[G] - img(r - 1, c + 1)[G] +
							img(r, c)[G] - img(r + 1, c - 1)[G] + img(r, c)[G] - img(r + 1, c + 1)[G]) / 8;
				if (X == B)
					img(r, c)[R] = clamp(col);
				else
					img(r, c)[B] = clamp(col);
			}
			else {
				int red, blue;
				if (img(r - 1, c)[R] != 0) {
					red = (img(r - 1, c)[R] + img(r + 1, c)[R]) / 2;
					blue = (img(r, c - 1)[B] + img(r, c + 1)[B]) / 2;
				}
				else {
					red = (img(r, c - 1)[R] + img(r, c + 1)[R]) / 2;
					blue = (img(r - 1, c)[B] + img(r + 1, c)[B]) / 2;
				}
				img(r, c)[R] = clamp(red);
				img(r, c)[B] = clamp(blue);
			}
		}
	}

}



int main() {

	mat<uint16_t> img16;
	const std::string fi = "alambracco";
	if (!load_pgm16(fi+".pgm", img16)) {
		error("Error loading file");
	}
	mat<uint8_t> img8;
	from16to8(img16, img8);
	mat<rgb> img;

	if (!save_pgm(fi+"_gray.pgm", img8)) {
		error("Error saving file");
	}

	to_rgb(img8, img);
	if (!save_ppm(fi + "_bayer.ppm", img)) {
		error("Error saving file");
	}

	green_interpolation(img);
	
	if (!save_ppm(fi + "_green.ppm", img)) {
		error("Error saving file");
	}

	RB_interpolation(img);

	if (!save_ppm(fi + "_interp.ppm", img)) {
		error("Error saving file");
	}
	return EXIT_SUCCESS;
}