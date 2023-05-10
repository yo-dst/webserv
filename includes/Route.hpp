#pragma once

#include <string>
#include <set>
#include <vector>
#include <stdexcept>

struct s_cgi
{
	std::string extension;
	std::string cgi_path;

	s_cgi(const std::string &ex)
		: extension(ex)
	{}

	s_cgi(const std::string &ex, const std::string &cp)
		: extension(ex), cgi_path(cp)
	{}

	s_cgi(const s_cgi &c)
	{
		*this = c;
	}

	s_cgi &operator=(const s_cgi &c)
	{
		if (this == &c)
			return (*this);
		extension = c.extension;
		cgi_path = c.cgi_path;
		return (*this);
	}

	bool operator<(const s_cgi &rhs) const
	{
		return (this->extension < rhs.extension);
	}
};

struct redirect_data
{
	int			code;
	std::string	url;
};

class Route
{
public:
	//Route(void);
	Route(const std::string &name);
	//Route(const Route &rhs);
	//Route &operator=(const Route &rhs);
	//~Route(void);
		
	void	addMethods(const std::vector<std::string> &methods);

	void	enableDirectoryListing(bool enabled);
	
	void	setLocalPath(const std::string &local_path);
	void	setIndex(const std::string &index_path);
	void	setRedirect(const std::string &url, int code);
	void	setCgi(const std::string &cgi_path, const std::string &extension);
	void	setUploads(const std::string &save_path);

	bool		isDirectoryListingEnabled(void) const;
	bool		isRedirect(void) const;
	bool		areUploadsEnabled(void) const;

	redirect_data	getRedirection(void) const;
	std::string		getUploadPath(void) const;

	bool			isCgiUri(std::string uri) const;
	std::string		getCgiBinaryForUri(std::string uri) const;

	bool		hasMethod(const std::string &method) const;
	int			uriMatchScore(const std::string &uri) const;

	std::string	uriToLocalFile(std::string uri) const;

	// PLEIN DAUTRES FONCTIONS UTILES
	std::string	getName(void) const;

private:
	bool																m_directory_listing;
	std::set<std::string>												m_methods;
	std::string															m_index;
	std::string 														m_local_path;
	std::string															m_route_name;
	redirect_data														m_redirect;
	bool																m_redirect_enabled;
	std::set<s_cgi>														m_cgi;
	struct {bool enabled; std::string save_path;}						m_uploads;
};

bool operator==(const Route &c, const std::string &name);
bool operator!=(const Route &c, const std::string &name);
