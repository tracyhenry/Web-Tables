#ifndef BRIDGE__H
#define BRIDGE__H

#include "KB.h"
#include "Corpus.h"
#include "TaxoPattern.h"
#include <map>
#include <vector>
#include <string>
#include <utility>
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

	//Initialize match result
	void initMatch();

	//Initialize the KB property
	void initKbProperty();

	//make the KB property recursively
	void makeSchema(int);

	//Initialize the taxo pattern of cells
	void initCellPattern();

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
};

#endif
