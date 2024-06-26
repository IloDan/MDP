#include<string>
#include<iostream>
#include<fstream>
#include<limits>
#include<vector>
#include "mat.h"
#include "pgm.h"
#include "ppm.h"
#include "types.h"
#include "utils.h"

void error(const std::string& msg) {
	std::cout << msg << '\n';
	exit(EXIT_FAILURE);
}


bool y4m_extract_color(const std::string& filename, std::vector<mat<vec3b>>& frames) {
	using namespace std;
	ifstream is(filename, ios::binary);
	if (!is)
		return false;

	string MagicNumber;
	is >> MagicNumber;
	if (MagicNumber != "YUV4MPEG2")
		error("errore apertura y4m");
	int H, W;
	string C, X, F, A;
	char I;
	while (is.peek() != '\n') {
		char x = is.get();
		if (x == 'H')
			is >> H;
		if (x == 'W')
			is >> W;
		if (x == 'C')
			is >> C;
		if (x == 'I')
			is >> I;
		if (x == 'F')
			is >> F;
		if (x == 'X')
			is >> X;
		if (x == 'A')
			is >> A;
	}
	is.get();
	string head(5, ' ');
	mat<vec3b> frame(H, W);

	vector<uint8_t> Y(frame.size()), CB(frame.size()/4), CR(frame.size()/4);

	vector<double> formula = { 1.164, 0, 1.596, 1.164, -0.392, -0.813, 1.164, 2.017, 0 };
	while (true) {

		is.read(reinterpret_cast<char*>(&head[0]), 5);
		if (!is)
			break;
		if (head != "FRAME")
			error("errore header non FRAME");
		is.ignore(numeric_limits<streamsize>::max(), '\n');

		is.read(reinterpret_cast<char*>(&Y[0]), frame.size());
		for (auto& x : Y) {
			if (x < 16)
				x = 16;
			else if (x > 235)
				x = 235;
		}
		is.read(reinterpret_cast<char*>(&CB[0]), CB.size());
		for (auto& x : CB) {
			if (x < 16)
				x = 16;
			else if (x > 240)
				x = 240;
		}
		is.read(reinterpret_cast<char*>(&CR[0]), CR.size());
		for (auto& x : CR) {
			if (x < 16)
				x = 16;
			else if (x > 240)
				x = 240;
		}

		for (int r = 0; r < frame.rows(); ++r) {
			for (int c = 0; c < frame.cols(); ++c) {

				int pos = (r * frame.cols() + c);
				int cpos =(r/2 * frame.cols()/2 + c/2) ;

				auto ybob = formula[0] * (Y[pos] - 16);

				auto cbob = formula[1] * (CB[cpos] - 128);

				auto crbob = formula[2] * (CR[cpos] - 128);
				
				double red =ybob + cbob +crbob;

				if (red < 0) 
					red = 0;
				else if (red > 255)
					red = 255;

				double green = formula[3] * (Y[pos] - 16) + formula[4] * (CB[cpos] - 128) + formula[5] * (CR[cpos] - 128);
				if (green < 0) green = 0;
				else if (green > 255)
					green = 255;

				double blue = formula[6] * (Y[pos] - 16) + formula[7] * (CB[cpos] - 128) + formula[8] * (CR[cpos] - 128);
				if (blue < 0) blue = 0;
				else if (blue > 255)
					blue = 255;
				
				frame(r, c)[0] = static_cast<uint8_t>(red);
				frame(r, c)[1] = static_cast<uint8_t>(green);
				frame(r, c)[2] = static_cast<uint8_t>(blue);
			}
		}

		frames.push_back(frame);
	}

	return true;
}


//int main() {
//
//	std::vector<mat<vec3b>> frames;
//
//	if (!y4m_extract_color("test1.y4m", frames)) {
//		error("errore caricamento immagine");
//	}
//
//	return EXIT_SUCCESS;
//}