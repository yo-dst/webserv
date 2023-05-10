#pragma once

#include <string>
#include <set>
#include <map>
#include <vector>

#include "Route.hpp"

/*
Informations pour utiliser la classe

Chaque server a un objet Config: m_config
Chaque server et config est mis dans un thread et initialise au debut, en meme temps.

Normalement on peut recuperer ce quon veut avec les getters, std::find est utilise quand il s'agit de trouver dans un container
Le port n'est pas en network byte order
Quand on utilise une fonction pour recuperer par nom/id et que ce n'est pas trouve, une chaine vide OU nullptr est return, mettre une exception si ca parait mieux?
(getRouteByName, getErrorPageByCode)
*/

//TODO: Check error code upon adding error page
// SI UNE VARIABLE NA PAS LAIR MODIFIEE QUAND LA CONFIG EST CHANGEE, SUREMENT UN BUG DU GETTER/SETTER ASSOCIE

class Config
{
	public:
		Config();
		~Config();

		Config(const Config &conf);
		Config &operator=(const Config &conf);

		void	setHost(const std::string &host);
		void	addName(const std::string &name);

		void	addPort(unsigned short port);

		void	setClientBodySize(int size);
		void	setPort(unsigned short port);

		void	addErrorPage(int code, const std::string &path);
		void	addRoute(const std::string &route);

		bool								hasName(const std::string &name) const;
		const std::set<std::string>			&getNames(void) const;

		std::string							getHost(void) const;
		unsigned short						getPort(void) const;
		std::vector<unsigned short>			getPorts(void) const;
		int									getClientBodySize(void) const;

		const std::map<int, std::string>	&getErrorPages(void) const;
		const std::string					getErrorPageByCode(int code) const;

		std::vector<Route> &				getRoutes(void);
		Route								*getRouteByName(const std::string &name);

		void								validate(void);
	private:
		std::string					m_host;
		std::set<std::string>		m_names;
		std::vector<unsigned short>	m_ports;
		std::map<int, std::string>	m_error_pages;
		int							m_client_body_size;
		std::vector<Route>			m_routes;
};