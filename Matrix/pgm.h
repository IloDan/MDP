#include<string>
#include "matrix.h"

enum class pgm_mode { plain, binary };
bool write(const std::string& filename, mat<uint8_t>& img, pgm_mode mode);