#pragma once

#include <list>
#include <algorithm>

#include <sys/event.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
    
#include <arpa/inet.h>

#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>

#include "Client.hpp"
#include "Logger.hpp"
#include "utils.hpp"
#include "Config.hpp"
#include "Route.hpp"

#define CLIENT_SAFE_GET(client, fd) \
	client = get_client(fd); \
	if (!client) \
	{ \
		remove_client(fd); WARN("tried to handle event for deleted client which had fd " << fd); continue ; \
	}

/*
	If the server is too slow, try using a std::set instead of std::list for the clients container, also need to overload bool Client::operator<(Client &);
*/

typedef std::vector<Config *> t_virtual_servers;

class Server
{
public:
	Server(t_virtual_servers virtual_servers);
	~Server();

	static void *static_start(void *server);

	void	process_requests();

	void			setPort(unsigned short p);

	unsigned short	getPort(void) const;
	std::string		getHost(void) const;
private:
	void	initialize_io(void);
	void	enable_listening(void);

	void	add_client(int fd, struct sockaddr_in client_addr);
	void	remove_client(int fd);
	Client	*get_client(int fd);
	void	wait_for_client_avaibility(int fd);

	Route *matchRoute(const std::string &uri);

	static const int				s_max_events = 64;
	int								m_kqueue;
	int								m_local_socket;
	std::list<Client>				m_clients;
	struct kevent					m_ev_list[s_max_events];
	bool							m_listening;
	t_virtual_servers				m_virtual_servers;
	const static uint64_t			s_request_timeout = 15000;
};