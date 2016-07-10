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

class Bridge
{
private:
	//Match Result: matches[cellId] = [KBentity1, KBentity2 ...]
	std::vector<std::vector<int>> matches;

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

	//get matches
	std::vector<int>& getMatch(int cellId);

	//Find the most similar concept for a record
	std::vector<int> findRecordConcept(int, int, bool);

	//Find the most possible type of relation for a given column
	std::vector<int> findRelation(int, int, bool);

	//Find the relations for all attr columns
	void findAllRelation();

	//Find the most similar concepts for all records
	void findAllConcept();

	//Naive method to get column concepts
	std::vector<int> findColConceptMajority(int, int, bool);
};

#endif
