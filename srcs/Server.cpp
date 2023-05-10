#include "../includes/Server.hpp"

Server::Server(t_virtual_servers virtual_servers) : m_listening(false), m_virtual_servers(virtual_servers)
{
	initialize_io();
}

Server::~Server()
{
	close(m_kqueue);
	close(m_local_socket);
}

void Server::enable_listening(void)
{
	safe_syscall(listen(m_local_socket, 128), "listen");
	m_listening = true;
	INFO("Server is now listening on port " << getPort());
	INFO(m_virtual_servers.size() << " virtual server are/is being served by this physical server");
}

unsigned short Server::getPort(void) const
{
	return (m_virtual_servers[0]->getPorts()[0]);
}

std::string Server::getHost(void) const
{
	return (m_virtual_servers[0]->getHost());
}

void Server::initialize_io()
{
	int yes;
	struct sockaddr_in s;

	memset(&s, 0, sizeof(s));
	s.sin_family = AF_INET;
	s.sin_addr.s_addr = inet_addr(getHost().c_str());
	s.sin_port = htons(getPort());
	this->m_local_socket = safe_syscall(socket(PF_INET, SOCK_STREAM, getprotobyname("tcp")->p_proto), "socket");

	if (setsockopt(this->m_local_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
	{
		perror("setsockopt");
		exit(1);
	}
	safe_syscall(bind(this->m_local_socket, (struct sockaddr *)&s, sizeof(s)), "bind");

	fcntl(m_local_socket, F_SETFL, O_NONBLOCK);

	// Make the queue and make it register connection events
	struct kevent ev_set;

	EV_SET(&ev_set, this->m_local_socket, EVFILT_READ, EV_ADD, 0, 0, NULL);
	m_kqueue = safe_syscall(kqueue(), "kqueue");
	safe_syscall(kevent(m_kqueue, &ev_set, 1, NULL, 0, NULL), "kevent");

	INFO("Socket and kqueue for server " << getHost() << ":" << getPort() << " initialized!");
}

void Server::add_client(int fd, struct sockaddr_in client_addr)
{
	struct kevent evSet[3];

	m_clients.push_back(Client(fd, client_addr));
	DEBUG("New client connected from " << m_clients.back().getAddress());
	EV_SET(evSet, m_clients.back().getSocket(), EVFILT_READ, EV_ADD, 0, 0, NULL);
	EV_SET(&evSet[1], m_clients.back().getSocket(), EVFILT_TIMER, EV_ADD | EV_ONESHOT, 0, Server::s_request_timeout, NULL);
	EV_SET(&evSet[2], m_clients.back().getSocket(), EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
	kevent(m_kqueue, evSet, 3, NULL, 0, NULL);
}

void Server::remove_client(int fd)
{
	struct kevent evSet;

	std::list<Client>::iterator it = std::find(m_clients.begin(), m_clients.end(), fd);

	EV_SET(&evSet, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
	kevent(m_kqueue, &evSet, 1, NULL, 0, NULL);
	EV_SET(&evSet, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
	kevent(m_kqueue, &evSet, 1, NULL, 0, NULL);
	EV_SET(&evSet, fd, EVFILT_TIMER, EV_DELETE, 0, Server::s_request_timeout, NULL);
	kevent(m_kqueue, &evSet, 1, NULL, 0, NULL);

	if (it != m_clients.end())
	{
		DEBUG("Client " << it->getAddress() << " (fd: " << it->getSocket() << ") "
						<< " disconnected");
		m_clients.erase(it);
	}
}

Client *Server::get_client(int fd)
{
	std::list<Client>::iterator it = std::find(m_clients.begin(), m_clients.end(), fd);
	if (it == m_clients.end())
		return (nullptr);
	return (&*it);
}

void Server::wait_for_client_avaibility(int fd)
{
	struct kevent evSet;

	EV_SET(&evSet, fd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
	kevent(m_kqueue, &evSet, 1, NULL, 0, NULL);
	EV_SET(&evSet, fd, EVFILT_TIMER, EV_DELETE, 0, Server::s_request_timeout, NULL);
	kevent(m_kqueue, &evSet, 1, NULL, 0, NULL);
}

void *Server::static_start(void *server)
{
	Server *s = reinterpret_cast<Server *>(server);
	while (1)
		s->process_requests();
	return (server);
}

void Server::process_requests(void)
{
	Client *c;
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	int ret;

	if (!m_listening)
		enable_listening();

	int events_len = kevent(this->m_kqueue, NULL, 0, (struct kevent *)&m_ev_list, Server::s_max_events, NULL);
	for (struct kevent *event = m_ev_list; event != m_ev_list + events_len; event++)
	{
		// Disconnection
		if (event->flags & EV_EOF)
		{
			remove_client(event->ident);
		}
		// New connection
		else if (static_cast<int>(event->ident) == this->m_local_socket)
		{
			while (event->data--)
			{
				int fd = accept(m_local_socket, (struct sockaddr *)&client_addr, &client_addr_len);
				try
				{
					this->add_client(fd, client_addr);
				}
				catch (std::exception &e)
				{
					WARN(e.what());
				}
			}
		}
		else if (event->flags & EV_CLEAR)
		{
			DEBUG("fd " << event->ident << " triggered timeout");
			CLIENT_SAFE_GET(c, event->ident);

			c->timeOut();
			this->remove_client(event->ident);
		}
		// Receive data
		else if (event->filter == EVFILT_READ)
		{
			CLIENT_SAFE_GET(c, event->ident);
			DEBUG("client " << c->getAddress() << " in EVFILT_READ" << " (fd: " << c->getSocket() << ")");
			ret = c->recData(*event);
			if (ret == -1 || ret == 0)
			{
				WARN("recv() returned 0 or -1 for client " << c->getAddress());
				this->remove_client(event->ident);
			}
			else
			{
				if (c->getWaitingForMore())
					continue;
				try
				{
					c->parseRequest(m_virtual_servers);
					DEBUG(std::endl
						  << c->getRequest()->getFinal());
					c->buildResponse();
				}
				catch (int errCode)
				{
					c->buildErrorResponse(errCode);
				}
				catch (std::exception &e)
				{
					DEBUG("client " + c->getAddress() + " throw unknow exception '" + e.what() + "'");
				}
				this->wait_for_client_avaibility(event->ident);
			}
		}
		else if (event->filter == EVFILT_WRITE)
		{
			CLIENT_SAFE_GET(c, event->ident);

			DEBUG("client " << c->getAddress() << " in EVFILT_WRITE");
			if (c->getResponse() != nullptr && c->getResponse()->isBuilt)
			{
				c->sendResponse();
				DEBUG(std::endl
					  << c->getResponse()->getFormatedHTTPMsg());
				remove_client(event->ident);
			}
		}
	}
}
