#ifndef KB__H
#define KB__H

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

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
	std::vector<int> level;
	std::vector<std::unordered_set<int>> recursivePossess;
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
	std::unordered_map<std::string, int> M;
	std::unordered_map<int, std::string> MM;

	//Transformation between entity id and string
	std::unordered_map<std::string, int> E;
	std::unordered_map<int, std::string> EE;

	//Transformation between relation id and string
	std::unordered_map<std::string, int> R;
	std::unordered_map<int, std::string> RR;

	//Type relationship
	std::vector<std::vector<int>> belongs;
	std::vector<std::vector<int>> possess;

	//Facts  first : relation name (e.g. actedIn),  second : entity ID
	std::vector<std::vector<std::pair<int, int>>> facts;

	//Input Files
	std::string conceptFileName, entityFileName;
	std::string typeFileName, subclassFileName;
	std::string relationFileName, factFileName;

	//For KATARA
	std::vector<int> relTripleCount;
	std::vector<std::unordered_map<int, int>> entPairTripleCount;

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
	int getPossessEntity(int, int);		//by concept id & index
	int getRecursivePossessCount(int);	//by concept id
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
	std::string toLower(std::string);
	std::unordered_set<int>& getRecursivePossessEntities(int);	//by conceptId
	int getDepth(int);		//by concept id
	int getLevel(int);

	//For KATARA
	int getRelTripleCount(int);	//by relation id
	int getEntPairTripleCount(int, int);	//by ent1, ent2
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

private:
	//Generate supplementary facts
	void initSupFacts();

	//lowercase M
	std::unordered_map<std::string, int> lowerM;
};

#endif
