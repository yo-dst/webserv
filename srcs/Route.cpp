#include "../includes/Route.hpp"
#include "../includes/utils.hpp"


Route::Route(const std::string &name)
	: m_directory_listing(false), m_redirect_enabled(false)
{
	m_route_name = name;
	m_uploads.enabled = false;
	m_uploads.save_path = "";
}
	
void	Route::addMethods(const std::vector<std::string> &methods)
{
	m_methods.insert(methods.begin(), methods.end());
}

void	Route::enableDirectoryListing(bool enabled)
{
	m_directory_listing = enabled;
}

void	Route::setLocalPath(const std::string &local_path)
{
	m_local_path = local_path;
}

void	Route::setIndex(const std::string &index_path)
{
	m_index = index_path;
}

void	Route::setRedirect(const std::string &url, int code)
{
	m_redirect.code = code;
	m_redirect.url = url;
	m_redirect_enabled = true;
}

void	Route::setCgi(const std::string &cgi_path, const std::string &extension)
{
	m_cgi.insert(s_cgi(extension, cgi_path));
}

void	Route::setUploads(const std::string &save_path)
{
	m_uploads.save_path = save_path;
	m_uploads.enabled = true;
}

std::string Route::getName(void) const
{
	return (m_route_name);
}

redirect_data	Route::getRedirection(void) const
{
	if (!m_redirect_enabled)
		throw std::logic_error("Redirection not enabled, use isRedirection first");
	return (m_redirect);
}

std::string	Route::getUploadPath(void) const
{
	if (!m_uploads.enabled)
		throw std::runtime_error("Uploads are not enabled, use areUploadsEnabled first");
	return (m_uploads.save_path);
}

int			Route::uriMatchScore(const std::string &uri) const
{
	if (m_route_name == "/")
		return 1;
	if (uri.find(m_route_name, 0) == 0) {
		if (uri != m_route_name) {
			if (uri[m_route_name.size()] != '/')
				return 0;
		}
		return (m_route_name.length());
	}
	return (0);
}

// POUR NOAH
/*	Index prioritaire sur le directory listing quand il y a les deux
	L'index est gere automatiquement
 	La fonction peut return un dossier, le caller doit verifier si
 	son return est un dossier, et si c'en est un, il doit le lister
 	si et seulement si la route a le directory listing active */
std::string	Route::uriToLocalFile(std::string uri) const
{
	// uri -> /test.html
	if (m_local_path.empty() && uri == "/") {
		return "./";
	}
	uri.replace(0, m_route_name.length(), m_local_path);

	// Index of route
	if (trimstr(m_local_path, "/") == trimstr(uri, "/") && m_index != "")
		return (m_local_path + m_index);
	else
		return (uri);
}

// POUR NOAH
bool	Route::isDirectoryListingEnabled(void) const
{
	return (m_directory_listing);
}

bool	Route::isRedirect(void) const
{
	return (m_redirect_enabled);
}

bool	Route::isCgiUri(std::string uri) const
{
	std::string extension = getExt(uri);
	extension = std::string(".") + extension;
	std::set<s_cgi>::iterator it = m_cgi.find(extension);
	return (it != m_cgi.end());
}

std::string		Route::getCgiBinaryForUri(std::string uri) const
{
	if (!isCgiUri(uri))
		throw std::logic_error("uri is not cgi script, please use isCgiUri prior to calling this func");
	std::string extension = uri.substr(uri.rfind("."));
	std::set<s_cgi>::iterator it = m_cgi.find(extension);
	return (it->cgi_path);
}

bool	Route::areUploadsEnabled(void) const
{
	return (m_uploads.enabled);
}

bool	Route::hasMethod(const std::string &method) const
{
	return (m_methods.find(trimstr(tolower(method), " ")) != m_methods.end());
}

bool operator==(const Route &r, const std::string &name)
{
	return (r.getName() == name);
}
bool operator!=(const Route &r, const std::string &name)
{
	return (!(r.getName() == name));
}
