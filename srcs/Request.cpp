#include "../includes/Request.hpp"

Request::Request() {
	matchedRoute = nullptr;
	serverConf = nullptr;
}

Request::Request(Request const & src) {
	*this = src;
}

Request::~Request() {}

Request &Request::operator=(Request const &src)
{
	if (this == &src)
		return *this;
	final_str = src.final_str;
	body = src.body;
	method = src.method;
	uri = src.uri;
	headers = src.headers;
	matchedRoute = src.matchedRoute;
	serverConf = src.serverConf;
	version = src.version;
	fileName = src.fileName;
	contentType = src.contentType;
	query = src.query;
	return *this;
}

void	Request::parseUri(void)
{
	if (uri.find("?") == std::string::npos)
		return;
	query = uri.substr(uri.find("?") + 1);
	uri.erase(uri.find("?"));
}

void Request::parseFirstLine(std::string line)
{
	std::istringstream iss(line);
	std::string s;
	std::getline(iss, s, ' ' );
	this->method = s;
	std::getline(iss, s, ' ' );
	this->uri = s;
	std::getline(iss, s, ' ' );
	this->version = s.substr(s.find_first_of('P') + 2, 3);
	parseUri();
}

int	Request::parseLine(bool parseFirst, std::string line)
{
	if (parseFirst)
		parseFirstLine(line);
	else
		parseHeader(line);
	return 0;
}

void	Request::giveStr(std::string str)
{
	final_str += str;
}

void	Request::bind_request_to_virtual_server(std::vector<Config*> &virtual_servers, std::string host)
{
	if (host[0])
		host = split(trimstr(host, "\n\r "), ":")[0];
	for (std::vector<Config*>::iterator it = virtual_servers.begin(); it != virtual_servers.end(); ++it)
	{
		if ((*it)->hasName(host))
		{
			serverConf = *it;
			return ;
		}
	}
	serverConf = virtual_servers[0];
}


void	Request::parse(std::vector<Config*> &virtual_servers)
{
	unlink("raw-request");
	strToFile(final_str, "raw-request");
	parseUtils();
	
	std::string host;
	try
	{
		host = this->headers.at("Host");
	}
	catch (std::exception &)
	{
		host = "";
	}
	this->bind_request_to_virtual_server(virtual_servers, host);
	parseBody();
}

std::string	Request::getVarFile(std::string str)
{
	std::string var;
	var = body.substr(body.find(str, 0) + str.size());
	var = var.substr(0, var.find("\n") - 1);
	if (var.back() == '"')
	if (matchedRoute->areUploadsEnabled() && uri == matchedRoute->getName())
	var.pop_back();
	return var;
}

void	Request::parseBody(void)
{
	matchedRoute = matchRoute(uri, serverConf);
	if (!matchedRoute)
		throw 404;
	if (matchedRoute->areUploadsEnabled() && uri == matchedRoute->getName() && method == "POST" && strncmp(headers["Content-Type"].c_str(),"multipart/form-data", strlen("multipart/form-data")) == 0)
	{
		contentType = getVarFile("Content-Type: ");
		fileName = getVarFile("filename=\"");
		removeFirstnLines(4);
		removeLastnLines(2);
	}
}

void	Request::removeFirstnLines(int nb)
{
	for(; nb; nb--)
		body = body.substr(body.find("\n") + 1);
}

void	Request::removeLastnLines(int nb)
{
	for (; nb; nb--)
		body = body.erase(body.rfind("\n"));
}

bool	Request::checkLengthHeader(void)
{
	size_t size = method.size() + uri.size() + version.size();
	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
		size += it->first.size() + it->second.size();
	if (size > 8190)
		return false;
	return true;
}

void Request::validate() {
	if (!httpProtocolVersionIsValid()) {
		throw 505;
	}
	if (!methodIsImplemented()) {
		throw 501;
	}
	if (uri.size() > 2048)
		throw 414;
	if (!checkLengthHeader())
		throw 431;
	matchedRoute = matchRoute(uri, serverConf);
	if (static_cast<unsigned long>(serverConf->getClientBodySize()) < body.size())
		throw 413;
	if (matchedRoute->isRedirect())
		if (matchedRoute->getRedirection().code != 307 && matchedRoute->getRedirection().code != 308)
			throw 503;
	if (!matchedRoute) {
		throw 404;
	}
	if (!matchedRoute->hasMethod(method)) {
		throw 405;
	}
}

bool	Request::httpProtocolVersionIsValid() const {
	if (strncmp(version.c_str(), "1.1", 3) != 0) {
		return false;
	} else {
		return true;
	}
}

bool	Request::methodIsImplemented() const {
	if (method != "POST" && method != "GET" && method != "DELETE") {
		return false;
	} else {
		return true;
	}
}

void	Request::parseUtils()
{
	size_t pos = 0;
	std::string token;
	std::string delimiter = "\r\n";
	std::string news = final_str;
	int i = 0;
	while ((pos = news.find(delimiter)) != std::string::npos) {
		token = news.substr(0, pos + 2);
		if (token == "\r\n" && i != 0)
			break;
		if (parseLine(i == 0 ? true : false, token) == 1)
			break;
		news.erase(0, pos + delimiter.length());
		i++;
	}
	if (i == 0)
		throw 400;
	body = news.erase(0, 2);
}

void Request::parseHeader(std::string line)
{
	std::string var = line.substr(line.find(":") + 2);
	var.pop_back();
	headers[line.substr(0, line.find(":"))] = var;
}

std::string Request::getFinal()
{
	return final_str;
}

Route const *	Request::getMatchedRoute() const {
	return matchedRoute;
}
