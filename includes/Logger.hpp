#pragma once

#include <iostream>
#include <string>
#include <ctime>

static inline std::string	get_time()
{
	time_t		rawtime;
	struct tm 	*timeinfo;
	char 		buffer[128];

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, sizeof(buffer), "[ %T ] ", timeinfo);
	return std::string(buffer);
}

#define COLOR_RED	 "\033[0;31m"
#define COLOR_GREEN	 "\033[0;32m"
#define COLOR_YELLOW "\033[0;33m"
#define COLOR_BLUE	 "\033[0;34m"
#define COLOR_MAGENTA "\033[0;35m"
#define COLOR_BOLD	 "\033[1m"
#define COLOR_GRAY	 "\033[1;30m"
#define COLOR_RESET  "\033[0m"

#define WHERE COLOR_GRAY << " (in: " << __PRETTY_FUNCTION__ << ", " << __FILE__ << ":" << __LINE__ << ")" << COLOR_RESET << " "

#define LOGLEVEL_FORMAT(msg, color) "[" COLOR_BOLD color msg COLOR_RESET "] "

#define WARN_FORMAT LOGLEVEL_FORMAT("WARN", COLOR_YELLOW)
#define FATAL_FORMAT LOGLEVEL_FORMAT("FAIL", COLOR_RED)
#define INFO_FORMAT LOGLEVEL_FORMAT("INFO", COLOR_GREEN)
#define DEBUG_FORMAT LOGLEVEL_FORMAT("DEBUG", COLOR_MAGENTA)
#define TODO_FORMAT LOGLEVEL_FORMAT("TODO", COLOR_BLUE)

#define WARN(msg) std::cout << get_time() << WARN_FORMAT << COLOR_BOLD << msg << COLOR_RESET << WHERE << std::endl
#define FATAL(msg) std::cerr << get_time() << FATAL_FORMAT << COLOR_BOLD << msg << COLOR_RESET << WHERE << std::endl, exit(1)
#define INFO(msg) std::cout << get_time() << INFO_FORMAT << COLOR_BOLD << msg << COLOR_RESET << std::endl
#define THROW(msg, status) std::cerr << get_time() << INFO_FORMAT << COLOR_BOLD << msg << COLOR_RESET << std::endl, throw status
#define TODO(msg) std::cout << get_time() << TODO_FORMAT << COLOR_BOLD << msg << COLOR_RESET << WHERE << std::endl

#ifdef DEBUGGING
	#define DEBUG(msg) std::cout << get_time() << DEBUG_FORMAT << COLOR_BOLD << msg << COLOR_RESET << std::endl
#else
	#define DEBUG(msg) (void)0
#endif
