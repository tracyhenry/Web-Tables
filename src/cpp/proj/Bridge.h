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

	//schema of kb concepts: kbProperty[conceptId](relationId, TaxoPattern)
	std::vector<std::unordered_map<int, TaxoPattern *>> kbProperty;

	//Taxo patterns of table cells
	std::vector<TaxoPattern *> cellPattern;

	//debug file
	std::ofstream debug;

	//Initialize match result
	void initMatch();

	//Initialize the KB property
	void initKbProperty();

	//make the KB property recursively
	void makeSchema(int);

	//Initialize the taxo pattern of cells
	void initCellPattern();

	//get kb Property: concept id, relation id, debug or not
	TaxoPattern *getKbProperty(int, int, bool);

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

	//Answer cell pattern queries
	void tableQuery();

	//Find the most similar concept for a record 
	void findConcept(int, int);
};

#endif
