#include <string>

std::string getExt(std::string const & path) {
	size_t pos = path.rfind('.');
	if (pos == std::string::npos) {
		return "";
	} else {
		size_t n = path.rfind('/');
		if (n != std::string::npos && n > pos) {
			return "";
		}
		return path.substr(pos + 1, path.size() - pos);
	}
}
