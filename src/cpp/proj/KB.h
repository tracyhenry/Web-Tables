#pragma once

#include <map>
#include <vector>
#include <string>


class KB
{
public:

	/**For graph**/
	//Adjacent lists
	std::vector<std::vector<int> > adj;
	
	//in-degree of nodes
	std::vector<int> inDegree;
	
	//number of concepts
	int N;
	
	/**Transformation between id and string**/
	std::map<std::string, int> M;
	std::map<int, std::string> MM;

	/**For topo-sort**/	
	std::vector<int> q;
	std::vector<int> used;
	int f;
	
	/**Public functions**/
	//Constructor
	KB();
	
};
