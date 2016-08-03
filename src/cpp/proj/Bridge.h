#ifndef BRIDGE__H
#define BRIDGE__H

#include "KB.h"
#include "Corpus.h"
#include "TaxoPattern.h"
#include <map>
#include <vector>
#include <string>
#include <utility>
#include <fstream>
#include <unordered_map>
#include <unordered_set>

struct KataraListEntry
{
	std::string type;
	int x, y;
	std::vector<std::pair<double, int>> candidates;

	KataraListEntry() {}
	KataraListEntry(std::string t, int _x, int _y) :
		type(t), x(_x), y(_y)
	{
		candidates.clear();
	}
};

class Bridge
{
private:

	//The weight for semi-lucky cells
	const double WT_SEMILUCKY = 0.25;

	//The threshold for the depth of a candidate concept
	const double TH_DEPTH = 12;

	//The maximum threshold for the proportion of lucky cells
	const double TMAX = 0.8;

	//The minimum threshold for the proportion of lucky cells
	const double TMIN = 0.4;

	//Match Result: matches[cellId] = [KBentity1, KBentity2 ...]
	std::vector<std::vector<std::pair<int, double>>> matches;

	//schema of kb concepts: conSchema[conceptId](relationId, TaxoPattern *)
	//schema of kb entities: entSchema[entityId](RelationId, TaxoPattern *)
	std::vector<std::unordered_map<int, TaxoPattern *>> conSchema;
	std::vector<std::unordered_map<int, TaxoPattern *>> entSchema;

	//Taxo patterns of KB entities
	std::vector<TaxoPattern *> entPattern;

	//Taxo patterns of table cells
	std::vector<TaxoPattern *> cellPattern;

	//Taxo patterns of columns
	std::vector<std::vector<TaxoPattern *>> colPattern;

	//Relations between attr columns and the entity column
	std::vector<std::vector<std::vector<int>>> colRelation;

	//Concepts for all records
	std::vector<std::vector<std::vector<int>>> recConcept;

	//debug file
	std::ofstream debug;

	//For KATARA
	std::vector<KataraListEntry> rankedLists;
	std::vector<double> ubs, sumUbs;
	std::vector<int> curState, ansState;
	std::vector<std::vector<double>> relSC;
	std::vector<std::vector<double>> p3;
	std::vector<double> p1, p2;
	std::vector<std::unordered_set<int>> entRels;
	double maxScore;

	//Initialize match result
	void initMatch();

	//Initialize KB entity patterns
	void initEntityPattern();

	//Initialize the KB schema
	void initKbSchema();

	//make the KB schema recursively
	void makeConSchema(int);

	//Initialize the taxo pattern of cells
	void initCellPattern();

	//get kb Schema: concept id, relation id, debug or not
	TaxoPattern *getKbSchema(int, int, bool);

	//get cell pattern : cellId, debug or not
	TaxoPattern *getCellPattern(int, bool);

	//print the map of a pattern
	void printPattern(TaxoPattern *);

	//get the number of lucky cells in a column, by curTable and cid
	double getNumLuckyCells(Table, int);

	//get the number of contained cells in a column by a concept, given by conceptId, curTable, cid
	double getNumContainedCells(Table, int, int);

	//katara private functions
	void kataraDfs(int);
	void kataraBackTrace(int, int, double);

public:
	//Knowledge base
	KB *kb;

	//Web table corpus
	Corpus *corpus;

	//Constructor
	Bridge();

	//Test function
	void testPattern();

	//Knowledge base traversal
	void traverse();

	//a function for debugging
	void letsDebug();

	//get matches
	std::vector<std::pair<int, double>>& getMatch(int cellId);

	/**
	* Our methods to find tuple concepts.
	*/
	//Find the most similar concept for a record
	std::vector<int> findRecordConcept(int, int, bool);

	//Find the most similar concepts for all records
	void findAllConcept();

	/**
	* Our methods to find column relationships.
	*/
	//Find the most possible type of relation for a given column
	std::vector<int> findRelation(int, int, bool);

	//Find the relations for all attr columns
	void findAllRelation();

	/**
	* Our methods to find column concepts.
	*/
	//Naive method to get column concepts
	std::vector<int> findColConceptMajority(int, int, bool);

	//Use taxonomy patterns to find column concepts & relations together
	std::vector<int> findColConceptAndRelation(int, bool);

	//Baseline
	std::vector<int> baselineFindColConceptAndRelation(int, bool);

	/**
	* KATARA related.
	*/
	void initRankedLists(int);
	void initCoherenceScores();
	std::vector<int> kataraFindColConceptAndRelation(int, bool);
};

#endif
