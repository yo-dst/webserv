#include "../includes/CGI.hpp"
#include "../includes/utils.hpp"
#include <fcntl.h>

CGI::CGI(Request &req) : m_req(req), m_conf(*req.serverConf) {}

CGI::~CGI() {}

void	CGI::InitEnv()
{
	m_map_env["SERVER_PORT"] = SSTR(m_conf.getPort());
	m_map_env["SERVER_SOFTWARE"] = "webserv/1.0";
	m_map_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	m_map_env["HTTP_ACCEPT_LANGUAGE"] = m_req.headers["Accept-Language"];
	m_map_env["HTTP_USER_AGENT"] = m_req.headers["User-Agent"];
	m_map_env["PATH_TRANSLATED"] = m_req.uri;
	m_map_env["PATH_INFO"] = m_req.uri;
	m_map_env["QUERY_STRING"] = m_req.query;
	m_map_env["REMOTE_HOST"] = m_conf.getHost();
	m_map_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	m_map_env["REQUEST_METHOD"] = m_req.method;
	m_map_env["HTTP_COOKIE"] = m_req.headers["Cookie"];
}

char ** build_args_for_script(std::string const & path_cgi, std::string const & path_script) {
	char **args = new char *[3];
	args[0] = new char[path_cgi.size() + 1];
	args[1] = new char[path_script.size() + 1];
	strcpy(args[0], path_cgi.c_str());
	strcpy(args[1], path_script.c_str());
	args[2] = NULL;
	return args;
}

std::string CGI::exec_script(std::string const & path_cgi, std::string const & path_script)
{
	InitEnv();
	int pipeFd[2];

	if (pipe(pipeFd) == -1) {
		throw 500;
	}
	int pid;
	if ((pid = fork()) == -1) {
		throw 500;
	}
	if (pid == 0) {
		char ** args = build_args_for_script(path_cgi, path_script);
		char ** env = MapToArr();
		close(pipeFd[0]);
		dup2(pipeFd[1], STDOUT_FILENO);
		close(pipeFd[1]);
		int inFd = safe_syscall(open(".cgi-tmp", O_RDWR | O_CREAT | O_CLOEXEC | O_TRUNC, 0775), "open");
		safe_syscall(write(inFd, m_req.body.c_str(), m_req.body.size()), "write");
		safe_syscall(dup2(inFd, STDIN_FILENO), "dup2");
		lseek(inFd, 0, SEEK_SET);
		execve(args[0], args, env);
		exit(EXIT_FAILURE);
	}
	int status;
	close(pipeFd[1]);
	waitpid(pid, &status, 0);
	if (WIFEXITED(status)) {
		if (WEXITSTATUS(status) != EXIT_SUCCESS) {
			close(pipeFd[0]);
			throw 500;
		}
	} else {
		close(pipeFd[0]);
		throw 500;
	}
	std::string data = readFd(pipeFd[0]); // can throw 500
	close(pipeFd[0]);
	return data;
}

char	**CGI::MapToArr()
{
	std::string line;
	int i = 0;
	char **env = new char *[m_map_env.size() + 1];
	for (std::map<std::string, std::string>::iterator it = m_map_env.begin(); it != m_map_env.end(); it++)
	{
		line = it->first + "=" + it->second;
		env[i] = new char[line.size() + 1];
		strcpy(env[i], line.c_str());
		i++;
	}
	env[i] = NULL;
	return env;
}