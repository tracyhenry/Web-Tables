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

	//simple dot product
	static double dotProduct(KB *, TaxoPattern *, TaxoPattern *);

	//using the depth of intersection with exponential weight
	static double expoDepth(KB *, TaxoPattern *, TaxoPattern *);
};

#endif

