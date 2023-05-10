#include <string>
#include <unistd.h>

bool fileExists(std::string const & path) {
	if (access(path.c_str(), F_OK) == -1) {
		return false;
	} else {
		return true;
	}
}
