#include <string>

std::string tolower(std::string str)
{
	for (size_t i = 0; i < str.length(); i++)
		str[i] = (str[i] >= 'A' && str[i] <= 'Z') ? (str[i] + 32) : str[i];
	return (str);
}
