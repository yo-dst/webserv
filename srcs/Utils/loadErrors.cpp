#include <map>
#include <string>

struct status {
	std::string reasonPhrase;
	std::string description;

	status(std::string const & reasonPhrase, std::string const & description) : reasonPhrase(reasonPhrase), description(description) {}
};

void loadStatus() {
	std::map<int, error> status;
	errors[400]
	errors[404] = error("");
}