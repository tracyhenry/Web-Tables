#ifndef KB__H
#define KB__H

#include <map>
#include <vector>
#include <string>


class KB
{
protected:

	//constants
	const std::string dirPath;
	const std::string delim;

	//Adjacent lists
	std::vector<std::vector<int>> adj;

	//number of concepts
	int N;

	//number of entities
	int K;

	//number of relations
	int F;

	//Transformation between concept id and string
	std::map<std::string, int> M;
	std::map<int, std::string> MM;

	//Transformation between entity id and string
	std::map<std::string, int> E;
	std::map<int, std::string> EE;

	//Transformation between relation id and string
	std::map<std::string, int> R;
	std::map<int, std::string> RR;

	//Type relationship
	std::vector<std::vector<int>> belongs;
	std::vector<std::vector<int>> possess;

	//Facts  first : relation name (e.g. actedIn),  second : entity ID
	std::vector<std::vector<std::pair<int, int>>> facts;

public:

	//Auxiliary function returning the total number of concepts in the KB
	int countConcept();

	//Public auxiliary functions
	int getAdjCount(int);		//by concept id
	int getAdjNode(int, int);	//by concept id & vector id

	//Constructor
	KB();

	//Basic Traverse
	virtual void traverse() = 0;

	//Deconstructor
	virtual ~KB() {}
};

class YAGO : public KB
{
private:
	//Input Files
	std::string conceptFileName, entityFileName;
	std::string typeFileName, subclassFileName;
	std::string relationFileName, factFileName;

public:
	//Constructor
	YAGO();

	//Init SubClassOf Relationship
	void initTaxonomy();

	//Init Type Relationship
	void initType();

	//Init Fact Relationship
	void initFact();

	//Auxiliary function to get concepts with most facts associated
	void getConceptWithMostFacts();

	//Extended Traverse Function
	void traverse();

};

#endif
