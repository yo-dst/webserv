#include "../../includes/utils.hpp"

std::string trimstr(std::string str, const std::string &charset)
{
	size_t pos;

	if (str.length())
	{
		if ((pos = str.find_first_not_of(charset)) != std::string::npos)
			str = str.substr(pos);
		if ((pos = str.find_last_not_of(charset)) != std::string::npos)
			str = str.substr(0, pos + 1);
	}
	return (str);
}
