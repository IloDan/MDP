//#include<string>
//#include<iostream>
//#include<fstream>
//#include<limits>
//
//#include "mat.h"
//#include "pgm.h"
//#include "ppm.h"
//#include "types.h"
//#include "utils.h"
//
//void error(const std::string& msg) {
//	std::cout << msg << '\n';
//	exit(EXIT_FAILURE);
//}
//
//
//bool y4m_extract_gray(const std::string& filename, std::vector<mat<uint8_t>>& frames) {
//	using namespace std;
//	ifstream is(filename, ios::binary);
//	if (!is)
//		return false;
//
//	string MagicNumber;
//	is >> MagicNumber;
//	if (MagicNumber != "YUV4MPEG2")
//		error("errore apertura y4m");
//	int H, W;
//	string C, X, F, A;
//	char I;
//	while (is.peek() != '\n') {
//		char x = is.get();
//		if (x == 'H')
//			is >> H;
//		if (x == 'W')
//			is >> W;
//		if (x == 'C')
//			is >> C;
//		if (x == 'I')
//			is >> I;
//		if (x == 'F')
//			is >> F;
//		if (x == 'X')
//			is >> X;
//		if (x == 'A')
//			is >> A;
//	}
//	is.get();
//	string head(5, ' ');
//	while(true){
//		
//		is.read(reinterpret_cast<char*>(&head[0]), 5);
//		if (!is)
//			break;
//		if (head != "FRAME") 
//			error("errore header non FRAME");
//		is.ignore(numeric_limits<streamsize>::max(), '\n');
//
//		mat<uint8_t> frame(H, W);
//		is.read(frame.rawdata(), frame.rawsize());
//		is.ignore(frame.size() / 2);
//		frames.push_back(frame);
//	}
//
//	return true;
//}
//
//
//int main() {
//
//	std::vector<mat<uint8_t>> frames;
//
//	if (!y4m_extract_gray("foreman_cif.y4m", frames)) {
//		error("errore caricamento immagine");
//	}
//
//	return EXIT_SUCCESS;
//}