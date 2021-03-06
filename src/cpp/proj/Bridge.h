#ifndef BRIDGE__H
#define BRIDGE__H

#include "KB.h"
#include "Corpus.h"
#include "TaxoPattern.h"
#include <map>
#include <queue>
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

	//Cache
	std::unordered_map<int, std::vector<int>> labelCache;

	//debug file
	std::ofstream debug;

	//For pruning algorithm
	std::priority_queue<std::pair<std::pair<double, double>, int>> heap;

	//matching rates
	std::unordered_map<int, std::vector<double>> colMR;
	std::vector<double> tableMR;

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

	//Initialize the taxo pattern of cells
	void initCellPattern();

	//Initialize matching rates
	void initMatchingRates();

	//get kb Schema: concept id, relation id, debug or not
	TaxoPattern *getKbSchema(int, int, bool);

	//get cell pattern : cellId, debug or not
	TaxoPattern *getCellPattern(int, bool);

	//print the map of a pattern
	void printPattern(TaxoPattern *);

	//get the number of lucky cells in a column, by curTable and cid
	double getNumLuckyCells(Table *, int);

	//get threshold
	double getThreshold(Table *, int);

	//katara private functions
	void kataraBackTrace(int, int, double);

	//recursive function for fast finding record concepts
	void dfsPrune(int, int, int, Table *, bool);

	//distance function which quantifies how far a concept deviates
	double distance(int, double, TaxoPattern *, TaxoPattern *);

	//sigma value for rec concept
	double sigma(int, int, int);

public:
	//Knowledge base
	KB *kb;

	//Web table corpus
	Corpus *corpus;

	//Constructor
	Bridge();

	//Cache clear
	void clearCache();

	//Test function
	void testPattern();

	//Knowledge base traversal
	void traverse();

	//a function for debugging
	void letsDebug();

	//get matches
	std::vector<std::pair<int, double>>& getMatch(int cellId);

	/**
	* Methods to find record concepts.
	*/
	//Find the top-K similar concepts for a record
	std::vector<int> findRecordConcept(int, int, int, bool, bool);

	//pruning algorithm
	std::vector<int> fastFindRecordConcept(int, int, int, bool, bool);

	//Baseline find record concept
	std::vector<int> baselineFindRecordConcept(int, int, int, bool, bool);

	//Find the most similar concepts for all records
	void findAllConcept();

	/**
	* Methods to find column relationships.
	*/
	//Baseline, most naive version
	int naiveFindRelation(int, int, bool);

	//Baseline using concept schema patterns
	std::vector<int> findRelation(int, int, bool);

	//Find the relations for all attr columns
	void findAllRelation();

	/**
	* Methods to find column concepts.
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

	/**
	* Enrichment related
	*/

	void enrichKB();
	void genFactTriple();
	void genAttrTypePair();
	void genEntityTypePair();
};

#endif
