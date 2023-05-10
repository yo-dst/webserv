#include "../includes/Server.hpp"
#include "../includes/Logger.hpp"
#include "../includes/utils.hpp"
#include "../includes/Config.hpp"

#include <stack>
#include <fstream>
#include <stdexcept>

#include <pthread.h>

// Ca cest du code pas opti qui copie des containers a balle
inline std::vector<std::vector<Config*> > getPhysicalServers(std::vector<Config*> &virtual_servers)
{
	std::vector<std::vector<Config*> > 			physical_servers;
	std::map<std::string, std::vector<Config*> >	physical_servers_map;

	for (std::vector<Config *>::iterator it = virtual_servers.begin(); it != virtual_servers.end(); ++it)
	{
		std::vector<unsigned short> ports = (*it)->getPorts();
		for (size_t i = 0; i < ports.size(); i++)
		{
			Config *c = new Config(*(*it));
			c->setPort(ports[i]);
			physical_servers_map[(*it)->getHost() + ":" + SSTR(ports[i])].push_back(c);
		}
		delete (*it);
	}
	for(std::map<std::string, std::vector<Config*> >::iterator it = physical_servers_map.begin(); it != physical_servers_map.end(); ++it ) {
        physical_servers.push_back(it->second);
    }
	return (physical_servers);
}

std::vector<pthread_t>	runServers(std::vector<Config*> &virtual_servers)
{
	pthread_t				th;
	std::vector<pthread_t>	threads;
	std::vector<std::vector<Config *> > physical_servers;

	physical_servers = getPhysicalServers(virtual_servers);
	for (std::vector<std::vector<Config *> >::iterator it = physical_servers.begin(); it != physical_servers.end(); ++it)
	{
		Server *s = new Server(*it);
		safe_syscall(pthread_create(&th, NULL, &Server::static_start, reinterpret_cast<void *>(s)), "pthread_create");
		threads.push_back(th);
	}
	return (threads);
}

void	expect(bool condition, const std::string &err, int line)
{
	if (!condition)
		throw std::runtime_error(err + SSTR(line));
}

bool	setScope(Config **conf, std::vector<std::string> str, std::stack<std::string> &scope, std::string &additionalScope, int lineNo)
{
	if (str[0] == "server")
	{
		expect(str.size() == 1, "wrong number of params server line ", lineNo);
		expect(!scope.size(), "server defined in wrong scope ", lineNo);
		scope.push("server");
		*conf = new Config();
		return (true);
	}
	else if (str[0] == "route")
	{
		expect(str.size() == 2, "wrong number of params route line ", lineNo);
		expect(scope.size() && scope.top() == "server", "route defined in wrong scope line ", lineNo);
		scope.push("route");
		additionalScope = str[1];
		(*conf)->addRoute(str[1]);
		return (true);
	}
	return (false);
}

std::vector<pthread_t> parseConfigAndRunServers(const char *fp)
{
	int							lineNo = 1;
	std::ifstream				conf(fp);
	std::stack<std::string>		scope;
	std::string					additionalScope = "";
	std::string					line;
	Config						*currentConf = nullptr;

 	std::vector<Config *>		virtual_servers;
	std::vector<pthread_t>		servers;

	if (!conf.good())
		throw std::runtime_error("Couldn't open file for reading " + std::string(fp));
	while (!conf.eof())
	{
		getline(conf, line);
		line = trimstr(line, " \t");

		// Handle scope exit
		if (line == "end")
		{
			expect(scope.size(), "orphan end on line ", lineNo);
			if (scope.top() == "server")
			{
				currentConf->validate();
				virtual_servers.push_back(currentConf);
			}
			scope.pop();
		}
		// Handle any other line that is not comment
		else if (line.length() && line[0] != '#')
		{
			std::vector<std::string> str = split(line, " \t");
			if (setScope(&currentConf, str, scope, additionalScope, lineNo))
				;
			else if (scope.top() == "server")
			{
				expect(str.size() >= 2, "invalid no of params line ", lineNo);

				// Handle error page
				if (str[0] == "error_page")
				{
					expect(str.size() == 3, "invalid error_page on line ", lineNo);
					currentConf->addErrorPage(std::stoi(str[1]), str[2]);
				}
				// Handle names
				else if (str[0] == "name")
					currentConf->addName(str[1]);
				// Handle port
				else if (str[0] == "port")
					currentConf->addPort(std::stoi(str[1]));
				// Handle host
				else if (str[0] == "host")
					currentConf->setHost(str[1]);
				else if (str[0] == "client_body_size")
					currentConf->setClientBodySize(std::stoi(str[1]));
				else
					throw std::runtime_error("unknown value on line " + SSTR(lineNo));
			}
			else if (scope.top() == "route")
			{
				Route *route = currentConf->getRouteByName(additionalScope);

				expect(str.size() >= 2, "invalid no of params line ", lineNo);
				if (str[0] == "methods")
				{
					std::vector<std::string> methods = split(str[1], ",");
					route->addMethods(methods);
				}
				else if (str[0] == "local_path")
					route->setLocalPath(str[1]);
				else if (str[0] == "directory_listing")
				{
					expect(str[1] == "on" || str[1] == "off", "invalid parameter for directory_listing on line ", lineNo);
					route->enableDirectoryListing(str[1] == "on" ? true : false);
				}
				else if (str[0] == "index")
					route->setIndex(str[1]);
				else if (str[0] == "redirect")
				{
					expect(str.size() == 3, "wrong number of parameters for redirect on line ", lineNo);
					route->setRedirect(str[1], std::stoi(str[2]));
				}
				else if (str[0] == "cgi")
				{
					expect(str.size() == 3, "wrong number of parameters for cgi on line ", lineNo);
					route->setCgi(str[1], str[2]);
				}
				else if (str[0] == "uploads")
				{
					expect(str.size() == 2, "wrong number of parameters for uploads on line ", lineNo);
					route->setUploads(str[1]);
				}
				else
					throw std::runtime_error("unknown value on line " + SSTR(lineNo));
			}
			else
				throw std::runtime_error("unknown value on line " + SSTR(lineNo));
		}
		lineNo++;
	}
	if (!scope.empty())
		throw std::runtime_error("unclosed block " + scope.top());
	servers = runServers(virtual_servers);
	return (servers);
}

int main(int argc, char **argv)
{
	if (argc <= 2)
	{
		try
		{
			std::vector<pthread_t> servers = parseConfigAndRunServers(argv[1] ? argv[1] : "config/default.conf");
			for (std::vector<pthread_t>::iterator it = servers.begin(); it != servers.end(); it++)
			{
				void *finished_server;
				safe_syscall(pthread_join(*it, &finished_server), "pthread_join");
				delete reinterpret_cast<Server *>(finished_server);
			}
			return (0);
		}
		catch (const std::exception &e)
		{
			FATAL("Config file error: " << e.what());
			return (1);
		}
	}
	else
		FATAL("Usage: " << argv[0] << " <config_file=\"config/default.conf\">");
}
