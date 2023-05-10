#include <fstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>

int strToFile(std::string const & data, std::string const & path) {
	if (access(path.c_str(), F_OK) == 0) {
		return -1;
	}
	std::ofstream out(path, std::ios::binary);
	out << data;
	out.close();
	chmod(path.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	return 0;
}
