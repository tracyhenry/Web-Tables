#ifndef MATCHER__H
#define MATCHER__H

#include "TaxoPattern.h"
#include "KB.h"
#include <unordered_map>

class Matcher
{
public:

	//using weighted jaccard
	static double weightedJaccard(KB *, TaxoPattern *, TaxoPattern *);

	//using depths of concepts
//	static double conceptLevelMatch(TaxoPattern *, TaxoPattern *);
};

#endif

