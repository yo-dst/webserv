#include <string>
#include <unistd.h>

int replaceStdinByStr(std::string const & str) {
	int pipeFd[2];
	if (pipe(pipeFd) == -1) {
		return -1;
	}
	if (write(pipeFd[1], str.c_str(), str.size()) == -1) {
		return -1;
	}
	close(pipeFd[1]);
	if (dup2(pipeFd[0], STDIN_FILENO) == -1) {
		return -1;
	}
	close(pipeFd[0]);
	return 0;
}
