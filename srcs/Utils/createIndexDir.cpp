#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sys/types.h>
#include <dirent.h>
#include <sstream>
#include <unistd.h>
#include <string.h>
#include <map>
#include "../../includes/Config.hpp"
#include "../../includes/utils.hpp"
#include <limits.h>

std::vector<std::string> getListOfFiles(std::string path)
{
	std::vector<std::string> files;
	DIR *d;
	struct dirent *dir;
	d = opendir(path.c_str());
	if (d) 
	{
		while ((dir = readdir(d)) != NULL)
		{
			if (strncmp(dir->d_name, ".", 2) == 0 || strncmp(dir->d_name, "..", 3) == 0)
				continue;
			files.push_back(dir->d_name);
		}
		closedir(d);
	}
	return files;
}

std::string	getIndexDir(std::string path, std::string uri, Config *conf)
{
	std::string			final_str;
	std::ostringstream	text;
	std::vector<std::string> files = getListOfFiles(path);	
	std::string			str_to_insert;

	(void) uri;
	(void)conf;
	std::ifstream f("resources/directory-listing-page.html");
	text << f.rdbuf();
	final_str = text.str();
	for (unsigned long i = 0; i != files.size(); i++)
	{	
		// "\t├── <a href=http://"+ SSTR(conf->getHost()) + ":"+ SSTR(conf->getPort()) +
		str_to_insert = "\t├── <a href=" + uri + ((uri[uri.size() - 1] == '/') ? "" : "/") + files[i] + ">" + files[i] + "</a><br>\n";
		// str_to_insert = "\t├── <a href=\"./" + files[i] + "\">" + files[i] + "</a><br>\n";
		final_str.insert(final_str.find("<h1>Directory Tree</h1><p>\n") + strlen("<h1>Directory Tree</h1><p>\n"), str_to_insert);
	}
	return final_str;
}