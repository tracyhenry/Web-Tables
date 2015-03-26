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
	//Match Result, (first, second) = (entityId, cellId)
	std::vector<std::pair<int, int>> matches;

	//TaxoPattern
	//What needs taxonomy pattern?
	//  1. Each cell
	//  2. Each property of each concept

	//schema of kb concepts: kbProperty[conceptId](relationId, TaxoPattern)
	std::vector<std::unordered_map<int, TaxoPattern *>> kbProperty;

	//Initialize match result
	void initMatch();

	//Initialize the KB property
	void initKbProperty();

	//make the KB property recursively
	void makeSchema(int);

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
};

#endif
