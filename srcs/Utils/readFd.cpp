#include <unistd.h>
#include <string>

std::string readFd(int fd) {
	char buff[64];
	int n;
	std::string data;
	bzero(buff, 64);
	while ((n = read(fd, buff, 63)) > 0) {
		data += buff;
		bzero(buff, 64);
	}
	if (n == -1) {
		throw std::runtime_error("read()");
	}
	return data;
}
