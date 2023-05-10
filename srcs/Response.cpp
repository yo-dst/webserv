#include "../includes/Response.hpp"

// --- public members ---

Response::Response(Request & req) : _req(req) {
	// _cgi = cgi;
	isBuilt = false;
}

Response::Response(Response & src) : _req(src._req) {
	// _cgi = src._cgi;
	_statusCode = src._statusCode;
	_headers = src._headers;
	_body = src._body;
	_formatedHTTPMsg = src._formatedHTTPMsg;
	isBuilt = false;
}

Response::~Response() {}

Response & Response::operator=(Response const & rhs) {
	_req = rhs._req;
	// _cgi = rhs._cgi;
	_statusCode = rhs._statusCode;
	_headers = rhs._headers;
	_body = rhs._body;
	_formatedHTTPMsg = rhs._formatedHTTPMsg;
	isBuilt = rhs.isBuilt;
	return *this;
}

void Response::build() {
	_clear();
	// set general headers
	_addHeader("Server", "webserv/1.0");
	_addHeader("Date", _getCurrDate());
	_addHeader("Connection", "close"); // should we handle persistent connections ?
	_addHeader("Cache-Control", "no-cache");
	_addHeader("Set-Cookie", "yummy_cookie=choco; secure");
	// handle the request
	if (_req.method == "GET") {
		_handleGet();
	} else if (_req.method == "POST") {
		_handlePost();
	} else if (_req.method == "DELETE") {
		_handleDelete();
	}
	_formatHTTPMsg();
	isBuilt = true;
}

void Response::buildError(int errCode) {
	_clear();
	_statusCode = errCode;
	_addHeader("Server", "webserv/1.0");
	_addHeader("Date", _getCurrDate());
	_addHeader("Connection", "close");
	_addHeader("Cache-Control", "no-cache");
	_buildErrorBody(errCode);
	_addHeader("Content-Length", SSTR(_body.size()));
	_formatHTTPMsg();
	isBuilt = true;
}

std::string const & Response::getFormatedHTTPMsg() const {
	return _formatedHTTPMsg;
}

// --- private members ---

void Response::_clear() {
	_formatedHTTPMsg.clear();
	_body.clear();
	_headers.clear();
	isBuilt = false;
}

void Response::_formatHTTPMsg() {
	_formatedHTTPMsg = _buildStatusLine();
	for (std::vector<HeaderItem>::iterator it = _headers.begin(); it != _headers.end(); ++it) {
		_formatedHTTPMsg += it->name + std::string(": ") + it->value + std::string("\n");
	}
	_formatedHTTPMsg += std::string("\n");
	_formatedHTTPMsg += _body;
}

void Response::_addHeader(std::string const & name, std::string const & value) {
	HeaderItem hi;
	hi.name = name;
	hi.value = value;
	_headers.push_back(hi);
}

void Response::_delHeader(std::string const & name) {
	std::vector<HeaderItem>::iterator ite = _headers.end();
	for (std::vector<HeaderItem>::iterator it = _headers.begin(); it != ite; ++it) {
		if (it->name == name) {
			_headers.erase(it);
		}
	}
}

std::string Response::_buildStatusLine() {
	std::string statusLine = std::string("HTTP/1.1 ") + SSTR(_statusCode);
	std::string reasonPhrase;
	switch (_statusCode) {
		case 200:
			reasonPhrase = "OK";
			break;
		case 201:
			reasonPhrase = "CREATED";
			break;
		case 307:
			reasonPhrase = "TEMPORARY REDIRECT";
			break;
		case 308:
			reasonPhrase = "PERMANENT REDIRECT";
			break;
		case 400:
			reasonPhrase = "BAD REQUEST";
			break;
		case 403:
			reasonPhrase = "FORBIDDEN";
			break;
		case 404:
			reasonPhrase = "NOT FOUND";
			break;
		case 405:
			reasonPhrase = "NOT ALLOWED";
			break;
		case 408:
			reasonPhrase = "REQUEST TIMEOUT";
			break ;
		case 414:
			reasonPhrase = "URI TOO LONG";
			break;
		case 431:
			reasonPhrase = "REQUEST HEADER FIELD TOO LARGE";
			break;
		case 500:
			reasonPhrase = "INTERNAL SERVER ERROR";
			break;
		case 501:
			reasonPhrase = "METHOD IMPLEMENTED";
			break;
		case 503:
			reasonPhrase = "SERVICE UNAVAILABLE";
			break;
		case 505:
			reasonPhrase = "HTTP version not supported";
			break;
		default:
			reasonPhrase = "";
			break;
	}
	statusLine += std::string(" ") + reasonPhrase + std::string("\n");
	return statusLine;
}

std::string Response::_getCurrDate() {
	char date[64];
	memset(date, '\0', 64);
	std::time_t t = std::time(nullptr);
	std::strftime(date, 64, "%a, %d %b %Y %X GMT", std::gmtime(&t));
	return std::string(date);
}

void Response::_buildErrorBody(int errCode) {
	std::string errPagePath = "";
	
	if (errCode == 408)
	{
		_addHeader("Content-Type", "text/html");
		_buildDefaultErrorBody();
		return ;
	}
	if (_req.serverConf)
		errPagePath = _req.serverConf->getErrorPageByCode(errCode);
	if (!errPagePath.empty()) {
		_addHeader("Content-Type", getMIMEType(errPagePath));
		_body = fileToStr(errPagePath.c_str());
	} else {
		_addHeader("Content-Type", "text/html");
		_buildDefaultErrorBody();
	}
}

void Response::_buildDefaultErrorBody() {
	std::string errMsg;
	switch (_statusCode) {
		case 400:
			errMsg = "bad request";
			break;
		case 403:
			errMsg = "access to file " + _req.uri + " is forbidden";
			break;
		case 404:
			errMsg = "file " + _req.uri + " not found";
			break;
		case 405:
			errMsg = _req.method + " not allowed for " + _req.uri;
			break;
		case 408:
			errMsg = "the request timed out";
			break ;
		case 414:
			errMsg = "uri " + _req.uri + " too long, it should not exceed 2048 characters";
			break;
		case 431:
			errMsg = "request header fields too large";
			break;
		case 500:
			errMsg = "internal server error";
			break;
		case 501:
			errMsg = "method " + _req.method +  " is not implemented";
			break;
		case 503:
			errMsg = "service unavailable, please try again later";
			break;
		case 505:
			errMsg = "HTTP protocol version " + _req.version + " is not supported, use HTTP/1.1 instead";
			break;
		default:
			errMsg = "";
			break;
	}

	_body = fileToStr("resources/default-error-page.html");
	size_t pos;
	while ((pos = _body.find("ERR_CODE")) != std::string::npos) {
		_body.replace(pos, 8, SSTR(_statusCode).c_str());
	}
	while ((pos = _body.find("ERR_MSG")) != std::string::npos) {
		_body.replace(pos, 7, errMsg);
	}
}

void Response::_redirect(Route const * matchedRoute)
{
	_statusCode = matchedRoute->getRedirection().code;
	_addHeader("Location", matchedRoute->getRedirection().url);
}

void Response::_handleGet() {
	_statusCode = 200;
	Route const * matchedRoute = _req.getMatchedRoute();
	std::string uriLocalPath = matchedRoute->uriToLocalFile(_req.uri);
	std::string contentType;
	std::string bodyContent;
	if(matchedRoute->isRedirect())
	{
		_redirect(matchedRoute);
		return;
	}	
	if (!fileExists(uriLocalPath))
		throw 404;
	if (!fileIsReadable(uriLocalPath))
		throw 403;
	if (fileIsDirectory(uriLocalPath))
	{
		if (matchedRoute->isDirectoryListingEnabled()) 
		{
			contentType = "text/html";
			bodyContent = getIndexDir(uriLocalPath, _req.uri, _req.serverConf);
		}
		else
			throw 403;
	}
	else if (matchedRoute->isCgiUri(_req.uri))
	{
		CGI cgi(_req);
		contentType = "text/html";
		bodyContent = cgi.exec_script(matchedRoute->getCgiBinaryForUri(_req.uri), uriLocalPath);
	}
	else
	{
		contentType = getMIMEType(uriLocalPath);
		bodyContent = fileToStr(uriLocalPath.c_str());
	}
	_body = bodyContent;
	_addHeader("Content-Type", contentType);
	_addHeader("Content-Length", SSTR(_body.size()));
}

void Response::_handlePost() {
	_statusCode = 200; // idk
	Route const * matchedRoute = _req.getMatchedRoute();
	if (matchedRoute->areUploadsEnabled() && _req.uri == matchedRoute->getName()) {
		std::string newFilePath = matchedRoute->getUploadPath() + "/" + _req.fileName;
		if (strToFile(_req.body, newFilePath) == -1) {
			throw 400;
		}
		else
			_statusCode = 201;
	} else {
		std::string uriLocalPath = matchedRoute->uriToLocalFile(_req.uri);
		if (!fileExists(uriLocalPath)) { // do we also check if file is readable ?
			throw 404;
		}
		if (matchedRoute->isCgiUri(_req.uri)) {
			CGI cgi(_req);
			_body = cgi.exec_script(matchedRoute->getCgiBinaryForUri(_req.uri), uriLocalPath);
			_addHeader("Content-Type", "text/html");
			_addHeader("Content-Length", SSTR(_body.size()));
		} else {
			throw 405;
		} 
	}
}

void Response::_handleDelete() {
	_statusCode = 200; // idk
	Route const * matchedRoute = _req.getMatchedRoute();
	std::string uriLocalPath = matchedRoute->uriToLocalFile(_req.uri);
	if (!fileExists(uriLocalPath)) {
		throw 404;
	}
	// maybe use remove, which can delete directories
	if (unlink(uriLocalPath.c_str()) == -1) { // uriLocalPath migth be a directory
		throw 403;
	}
}
