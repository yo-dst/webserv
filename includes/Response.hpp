#pragma once
#include "Request.hpp"
#include "utils.hpp"
#include <vector>
#include <ctime>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include "CGI.hpp"

// 3 kind of headers:
// 	- General headers
// 	- Response headers
// 	- Representation headers

// Bodies can be broadly divided into 3 categories:
// 	- single-resource bodies, consisting of a single file of known length -> uses two headers Content-Type and Content-Length
// 	- single-resource bodies, consisting of a single file of unknown length -> encoded by chunks with Transfer-Encoding set to 'chunked'
//	- multiple-resource bodies, this is typically associated with HTML forms (relatively rare)

struct HeaderItem {
	std::string name;
	std::string value;
};

// a Response is constructed from a given Request
class Response {
	public:
		bool isBuilt;
	private:
		int 							_statusCode;
		std::vector<HeaderItem>			_headers;
		std::string 					_body;
		std::string						_formatedHTTPMsg;
		Request &						_req;
		// CGI								_cgi;

	public:
		Response(Request & req);
		Response(Response & src);
		~Response();

		Response & operator=(Response const & rhs);

		void build();
		void buildError(int errCode);
		std::string const & getFormatedHTTPMsg() const;

	private:
		Response();

		void _clear();
		void _formatHTTPMsg();
		void _addHeader(std::string const & name, std::string const & value);
		void _delHeader(std::string const & name);
		std::string _buildStatusLine();
		std::string _getCurrDate();
		void _buildErrorBody(int errCode);
		void _buildDefaultErrorBody();
		void _redirect(Route const * matchedRoute);
		void _handleGet();
		void _handlePost();
		void _handleDelete();
};