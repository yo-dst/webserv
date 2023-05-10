#include <string>
#include <unistd.h>

bool fileIsReadable(std::string const & path) {
	if (access(path.c_str(), R_OK) == -1) {
		return false;
	} else {
		return true;
	}
}
