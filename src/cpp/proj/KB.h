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
	std::vector<std::vector<int>> pre, suc;

	//DFS stuff
	std::vector<int> depth, startTime, endTime;
	std::vector<int> recursivePossess;
	int timeStamp;

	//number of concepts
	int N;

	//number of entities
	int K;

	//number of relations
	int F;

	//root of the KB tree
	int root;

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

	//Input Files
	std::string conceptFileName, entityFileName;
	std::string typeFileName, subclassFileName;
	std::string relationFileName, factFileName;

	//Init SubClassOf Relationship
	void initTaxonomy();

	//Init Type Relationship
	void initType();

	//Init Fact Relationship
	void initFact();

	//dfs
	void doDFS(int);

public:
	//Public auxiliary functions
	int countConcept();
	int countEntity();
	int countRelation();
	int getRoot();
	int getPreCount(int);			//by concept id
	int getPreNode(int, int);		//by concept id & index
	int getSucCount(int);			//by concept id
	int getSucNode(int, int);		//by concept id & index
	int getBelongCount(int);		//by entity id
	int getBelongConcept(int, int);		//by entity id & index
	int getPossessCount(int);		//by concept id
	int getRecursivePossessCount(int);	//by concept id
	int getPossessEntity(int, int);		//by concept id & index
	int getFactCount(int);		//by entity id
	int getConceptId(std::string);		//by string
	int getEntityId(std::string);		//by string
	int getRelationId(std::string);		//by string
	int getReverseRelationId(int id);   //by relation ID;
	bool isDescendant(int, int);	//by childConceptId & fatherConceptId
	bool checkBelong(int, int);		//by entityId & conceptId
	bool checkRecursiveBelong(int, int);	//by entityId & conceptId
	std::pair<int, int> getFactPair(int, int);		//by entity id & index
	std::string getConcept(int); 		//by id
	std::string getEntity(int);		//by id
	std::string getRelation(int);		//by id
	int getDepth(int);		//by concept id
	//Constructor
	KB();

	//Destructor
	virtual ~KB() {}
};

class YAGO : public KB
{
public:
	//Constructor
	YAGO();

	//Auxiliary function to get concepts with most facts associated
	void getConceptWithMostFacts();
};

#endif
