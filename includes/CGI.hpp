#include <string>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <iostream>
#include <map>
#include "Request.hpp"
#include <dirent.h>

class CGI
{
	public:
		CGI(Request &req);
		~CGI(void);

		void InitEnv();
		char **MapToArr();
		std::string exec_script(std::string const & path_cgi, std::string const & path_script);

	private:
		std::map<std::string, std::string>	m_map_env;
		Request								&m_req;
		Config								m_conf;

};