#ifndef KB__H
#define KB__H

#include <map>
#include <vector>
#include <string>


class KB
{
public:

	/**For graph**/
	//Adjacent lists
	std::vector<std::vector<int> > adj;
	
	//number of concepts
	int N;
	
	/**Transformation between id and string**/
	std::map<std::string, int> M;
	std::map<int, std::string> MM;

	//Basic Traverse 
	virtual void Traverse() = 0;

	virtual ~KB() {}
};

class YAGO : public KB
{
public:

	/** Public functions **/
	//Constructor
	YAGO();	
	
	//Init SubClassOf Relationship
	void InitTaxonomy();
	
	//Init Type Relationship
	void InitType();
	
	//Extended Traverse Function
	void Traverse();
	
};

#endif
