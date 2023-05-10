#include "../includes/Client.hpp"

Client::Client(int fd, struct sockaddr_in addr)
{
	m_socket = fd;
	m_addr = addr;
	m_res = nullptr;
	m_req = nullptr;
	m_waiting_for_more = false;
	if (!valid())
		throw InvalidClientException();
}

Client::Client(const Client & rhs)
{
	m_res = nullptr;
	m_req = nullptr;
	*this = rhs;
}

Client & Client::operator=(const Client & rhs)
{
	if (this == &rhs)
		return (*this);
	m_socket = dup(rhs.m_socket);
	m_addr = rhs.m_addr;
	m_waiting_for_more = rhs.m_waiting_for_more;
	if (m_req) {
		delete m_req;
	}
	m_req = rhs.m_req ? new Request(*rhs.m_req) : nullptr;
	if (m_res) {
		delete m_res;
	}
	m_res = rhs.m_res ? new Response(*rhs.m_res) : nullptr;
	return (*this);
}

Client::~Client()
{
	close(m_socket);
	delete m_req;
	delete m_res;
}

bool	Client::valid(void) const
{
	return (getSocket() != -1);
}

int		Client::getSocket(void) const
{
	return (m_socket);
}

std::string	Client::getAddress(void) const
{
	std::string addr(inet_ntoa(m_addr.sin_addr));
	std::ostringstream oss;
	
	oss << ":" << ntohs(m_addr.sin_port);
	addr += oss.str();
	return (addr);
}

void	Client::parseRequest(std::vector<Config*> &virtual_servers)
{
	m_req->parse(virtual_servers);
	m_req->validate();
}

void	Client::buildResponse(void)
{
	if (!m_res) {
		m_res = new Response(*m_req);
	}
	m_res->build();
}

void	Client::timeOut(void)
{
	this->buildErrorResponse(408);
	this->sendResponse();
}


// Segfault is here, m_req must be null pointer when called by Client::timeOut(), maybe try to remove the req because it doesnt actually need anything to send back a 408
void Client::buildErrorResponse(int errCode) {
	if (!m_res) {
		m_res = new Response(*m_req);
	}
	m_res->buildError(errCode);
}

void	Client::sendResponse()
{
	size_t total_bytes = m_res->getFormatedHTTPMsg().size();
	ssize_t bytes_sent = send(m_socket, m_res->getFormatedHTTPMsg().c_str(), total_bytes, 0);
	if (bytes_sent == -1) {
		WARN("send() failure for " + getAddress());
	} 
	// else {
	// 	std::string msg = std::string("sent ") + SSTR(bytes_sent) + "/" + SSTR(total_bytes) + std::string(" bytes to ") + getAddress();
	// 	INFO(msg);
	// 	std::cout << "--- REQUEST ---" << std::endl \
	// 		 << m_req->getFinal() << std::endl \
	// 		<< "--- RESPONSE ---" << std::endl \
	// 		<< m_res->getFormatedHTTPMsg() << std::endl;
	// }
}

Response * Client::getResponse() {
	return m_res;
}

Request * Client::getRequest() {
	return m_req;
}

bool	Client::getWaitingForMore()
{
	return m_waiting_for_more;
}

int	Client::recData(struct kevent &event)
{
	size_t len = event.data;
	int r;
	char *buf = new char[len + 1];

	bzero(buf, len + 1);
	r = recv(m_socket, buf, len, 0);
	if (!m_req) {
		m_req = new Request();
		m_req->contentLength = getLength(buf);
		// timer begin
	}

	m_req->final_str += std::string(buf, len);
	if (m_req->contentLength != -1 && getBody(m_req->final_str).size() < (unsigned long)m_req->contentLength) {
		m_waiting_for_more = true;
	} else {
		// timer end
		m_waiting_for_more = false;
		// std::cout << "body size=" << getBody(m_req->final_str).size() << std::endl;
		// std::cout << "content-length=" << SSTR(m_req->contentLength) << std::endl;
	}
	// if (m_listening == false)
	// {
	// 	if (getBoundry() == false || NbBoundry() == true)
	// 		return;
	// }
	// else if (foundBoundry() != 3 && m_listening == true)

	return r; // wut ?
}

bool operator==(const Client &c, const int fd)
{
	return (c.getSocket() == fd);
}
bool operator!=(const Client &c, const int fd)
{
	return (!(c.getSocket() == fd));
}

const char *Client::InvalidClientException::what() const throw()
{
	return "Invalid client";
}