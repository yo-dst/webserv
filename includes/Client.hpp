#pragma once

#include <queue>
#include <string>
#include <sstream>
#include <iostream>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

#include "utils.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Config.hpp"

// if Content-Encoding value isn't supported throw 415 (Unsupported Media Type)
// actually idk if we're supposed to handle Content-Encoding

class Client
{
public:
	Client(int fd, struct sockaddr_in addr);
	Client(const Client &rhs);
	~Client(void);

	Client &operator=(const Client &rhs);

	bool		valid(void) const;
	int			getSocket(void) const;
	std::string	getAddress(void) const;
	void		buildResponse();
	void		buildErrorResponse(int errCode);
	void		timeOut(void);
	int			recData(struct kevent &);
	Route 		*matchRoute(const std::string &uri);
	void		parseRequest(std::vector<Config*> &virtual_servers);
	void		sendResponse();
	Response *	getResponse();
	Request * 	getRequest();
	bool		getWaitingForMore();

	class InvalidClientException : public std::exception
	{
		virtual const char *what(void) const throw();
	};

private:
	Client(void);

	bool				m_waiting_for_more;
	int					m_socket;
	struct sockaddr_in	m_addr;
	Config *			m_conf;
	Request	*			m_req;
	Response *			m_res;
};

bool operator==(const Client &c, const int fd);
bool operator!=(const Client &c, const int fd);