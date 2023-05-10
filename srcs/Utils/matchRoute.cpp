#include "../../includes/Config.hpp"

Route *matchRoute(const std::string &uri, Config * conf)
{
	std::vector<Route> & routes = conf->getRoutes();
	Route *r = nullptr;
	int highestScore =0;

	for (size_t i = 0; i < routes.size(); i++)
	{
		Route * curr = &(routes[i]);
		int match_score = curr->uriMatchScore(uri);
		if (match_score > highestScore)
		{
			r = curr;
			highestScore = match_score;
		}
	}
	return (r);
}
