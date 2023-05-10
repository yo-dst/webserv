#pragma once

#include "Logger.hpp"
#include <string>
#include <sstream>
#include <sys/errno.h>
#include <vector>
#include <string>
#include "Config.hpp"

extern int errno;

template<typename T>
T safe_syscall(T ret, std::string sysname)
{
	if (ret == -1)
		FATAL("syscall " + sysname + " failure ! (" + strerror(errno) + ")");
	return (ret);
}

#define SSTR( x ) static_cast< std::ostringstream & >( \
    ( std::ostringstream() << std::dec << x ) ).str()

std::string trimstr(std::string str, const std::string &charset);
std::vector<std::string> split(std::string toSplit, const std::string &charset);
std::string getMIMEType(std::string const & uri);
std::string getIndexDir(std::string, std::string, Config *);
std::string fileToStr(char const * filename);
std::string tolower(std::string str);
Route *matchRoute(const std::string &uri, Config * conf);
std::string readFd(int fd);
int strToFile(std::string const & data, std::string const & path);
bool fileIsDirectory(std::string const & path);
bool fileExists(std::string const & path);
bool fileIsReadable(std::string const & path);
int replaceStdinByStr(std::string const & str);
std::string getExt(std::string const & path);
int getLength(std::string const);
std::string getBody(std::string const &str);