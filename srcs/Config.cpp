#include "../includes/Config.hpp"
#include "../includes/Logger.hpp"

Config::Config(void)
{
	m_names.insert("");
}

Config::~Config(void)
{}

Config::Config(const Config &rhs)
{
	*this = rhs;
}

Config &Config::operator=(const Config &rhs)
{
	if (this == &rhs)
		return (*this);
	m_host = rhs.m_host;
	m_names = rhs.m_names;
	m_ports = rhs.m_ports;
	m_error_pages = rhs.m_error_pages;
	m_client_body_size = rhs.m_client_body_size;
	m_routes = rhs.m_routes;
	return (*this);
}

void	Config::validate(void)
{
	if (m_host == "" || !m_ports.size() || !m_routes.size())
		throw std::runtime_error("couldn't validate config file");
}

void	Config::setHost(const std::string &host)
{
	m_host = host;
}

void	Config::addName(const std::string &name)
{
	m_names.insert(name);
}
void	Config::setClientBodySize(int size)
{
	m_client_body_size = size;
}

void	Config::setPort(unsigned short port)
{
	m_ports = std::vector<unsigned short>(1, port);
}

void	Config::addPort(unsigned short port)
{
	if (std::find(m_ports.begin(), m_ports.end(), port) != m_ports.end())
		throw std::runtime_error("Server can't listen on same port multiple times");
	m_ports.push_back(port);
}
void	Config::addErrorPage(int code, const std::string &path)
{
	m_error_pages[code] = path;
}
void	Config::addRoute(const std::string &route)
{
	if (getRouteByName(route) != nullptr)
		throw (std::runtime_error("route `" + route + "` already exists"));
	m_routes.push_back(Route(route));
}

bool								Config::hasName(const std::string &name) const
{
	std::set<std::string>::const_iterator it = m_names.find(name);
	if (it != m_names.end())
		return (true);
	return (false);
}
const std::set<std::string>			&Config::getNames(void) const
{
	return (m_names);
}
std::string							Config::getHost(void) const
{
	return (m_host);
}

std::vector<unsigned short>			Config::getPorts(void) const
{
	return (m_ports);
}

unsigned short						Config::getPort(void) const
{
	return (m_ports[0]);
}


int									Config::getClientBodySize(void) const
{
	return (m_client_body_size);
}
const std::map<int, std::string>	&Config::getErrorPages(void) const
{
	return (m_error_pages);
}
const std::string					Config::getErrorPageByCode(int code) const
{
	std::map<int, std::string>::const_iterator it = m_error_pages.find(code);
	if (it != m_error_pages.end())
		return (it->second);
	return ("");

}
std::vector<Route> & Config::getRoutes(void)
{
	return (m_routes);
}

Route								*Config::getRouteByName(const std::string &name)
{
	if (m_routes.size())
	{
		std::vector<Route>::iterator it = std::find(m_routes.begin(), m_routes.end(), name);
		if (it != m_routes.end())
			return (&*it);
	}
	return (nullptr);
}
