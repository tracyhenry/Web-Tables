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
	std::vector<std::vector<int> > adj;

	//number of concepts
	int N;

	//number of entities
	int K;

	//Transformation between concept id and string
	std::map<std::string, int> M;
	std::map<int, std::string> MM;

	//Transformation between entity id and string
	std::map<std::string, int> E;
	std::map<int, std::string> EE;

	//Type relationship
	std::vector<std::vector<int> > belongs;
	std::vector<std::vector<int> > possess;

public:
	//Constructor
	KB();

	//Basic Traverse 
	virtual void Traverse() = 0;

	//Deconstructor
	virtual ~KB() {}
};

class YAGO : public KB
{
private:
	//Input Files
	std::string conceptFileName, entityFileName;
	std::string typeFileName, subclassFileName;
	std::string factFileName;

public:
	//Constructor
	YAGO();

	//Init SubClassOf Relationship
	void InitTaxonomy();

	//Init Type Relationship
	void InitType();

	//Init Fact Relationship
	void InitFact();

	//Extended Traverse Function
	void Traverse();

};

#endif
