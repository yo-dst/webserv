#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <cstring>
#include "utils.hpp"
#include <fstream>
#include <sstream>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <map>
#include "Route.hpp"
#include "Config.hpp"

class Request
{
	public:
		std::string							method;
		std::string							uri;
		std::string							version;
		std::map<std::string, std::string>	headers;
		std::string 						body;
		std::string							fileName; // yo: need explications
		Route *								matchedRoute;
		Config *							serverConf;
		std::string							contentType;
		std::string 						final_str;
		std::string							query;
		std::string							boundary;
		int									contentLength;

	private:

	public:
		Request(void);
		Request(Request const & src);
		~Request(void);

		Request &operator=(Request const &copy);

		void			giveStr(std::string);
		std::string 	getFinal(void);
		void			parse(std::vector<Config*> &virtual_servers);
		Route const *	getMatchedRoute() const;
		void 			validate();

	private:
		int				parseLine(bool, std::string);
		void			parseUri(void);
		void			parseFirstLine(std::string);
		void			parseHeader(std::string);
		void			parseUtils(void);

		bool			checkBodySize(void);
		bool			checkLengthHeader(void);
		bool			methodIsImplemented() const;
		bool			httpProtocolVersionIsValid() const;

		//parseBody
		void			parseBody(void);
		std::string		getVarFile(std::string);
		void			removeFirstnLines(int);
		void			removeLastnLines(int);

		void			bind_request_to_virtual_server(std::vector<Config*> &virtual_servers, std::string host);
};
