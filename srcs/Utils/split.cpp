#include <vector>
#include <string>
#include <iostream>

std::vector<std::string> split(std::string toSplit, const std::string &charset)
{
	std::vector<std::string>	words;
	std::size_t					pos;

	while ((pos = toSplit.find_first_of(charset)) != std::string::npos) {
		words.push_back(toSplit.substr(toSplit.find_first_not_of(charset), pos));
		toSplit.erase(0, pos + 1);
	}
	if (toSplit.length())
		words.push_back(toSplit);
	return (words);
}
