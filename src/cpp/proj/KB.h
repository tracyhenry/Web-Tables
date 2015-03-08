#ifndef KB__H
#define KB__H

#include <map>
#include <vector>
#include <string>


class KB
{
public:
	
	//static constants
	const std::string dirPath; 
	const std::string delim;
	
	//Adjacent lists
	std::vector<std::vector<int> > adj;
	
	//number of concepts
	int N;
	
	//number of entities
	int K;
	
	/**Transformation between concept id and string**/
	std::map<std::string, int> M;
	std::map<int, std::string> MM;

	/**Transformation between entity id and string**/
	std::map<std::string, int> E;
	std::map<int, std::string> EE;
	
	//Type relationship
	std::vector<std::vector<int> > belongs;
	std::vector<std::vector<int> > possess;

	//Constructor
	KB();
	
	//Basic Traverse 
	virtual void Traverse() = 0;

	//Deconstructor
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
