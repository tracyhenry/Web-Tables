#ifndef BRIDGE__H
#define BRIDGE__H

#include "KB.h"
#include "Corpus.h"
#include "TaxoPattern.h"
#include <map>
#include <vector>
#include <string>
#include <utility>

class Bridge
{
private:
	//Knowledge base
	KB *kb;

	//Web table corpus
	Corpus *corpus;

	//Match Result, (first, second) = (entityId, cellId)
	std::vector<std::pair<int, int>> matches;

	//TaxoPattern
	//What needs taxonomy pattern?
	//	1. Each cell
	//  2. Each instance
	//  2. Each property of each concept

public:

	//Constructor
	Bridge();

	//Test function
	void testPattern();
};

#endif
