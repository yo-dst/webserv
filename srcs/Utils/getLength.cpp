#include "../../includes/Client.hpp"

int getLength(std::string const str)
{
    std::string length;
    size_t pos;
    if ((pos = str.find("Content-Length")) == std::string::npos)
        return -1;
    length = str.substr(pos + 16);
    length = length.substr(0, length.find("\r\n"));
    return std::atoi(length.c_str());
}

std::string getBody(std::string const &str)
{
    std::string body = str;
    return body.erase(0, body.find("\r\n\r\n") + 4);
}