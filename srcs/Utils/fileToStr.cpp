#include <fstream>
#include <sstream>

std::string fileToStr(char const * filename) {
	std::ifstream ifs(filename);
	std::stringstream buff;
	buff << ifs.rdbuf();
	return buff.str();
}
