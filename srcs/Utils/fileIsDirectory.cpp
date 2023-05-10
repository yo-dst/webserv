#include <string>
#include <sys/errno.h>
#include <sys/stat.h>
#include <iostream>

bool fileIsDirectory(std::string const & path) {
	struct stat s;
	if (stat(path.c_str(), &s) == -1) {
		return false;
	}
	if (s.st_mode & S_IFDIR) {
		return true;
	} else {
		return false;
	}
}
