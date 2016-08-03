#ifndef MATCHER__H
#define MATCHER__H

#include "Bridge.h"
#include <unordered_map>

class Matcher
{
public:
	//general sim function
	static double patternSim(KB *, TaxoPattern*, TaxoPattern *);

	friend void Bridge::letsDebug();

private:
	//an exponential weight
	static double M;

	//using weighted jaccard
	static double weightedJaccard(KB *, TaxoPattern *, TaxoPattern *);

	//simple dot product
	static double dotProduct(KB *, TaxoPattern *, TaxoPattern *);

	//using the depth of intersection with constant-based exponential weight
	static double constExpoDepth(KB *, TaxoPattern *, TaxoPattern *);

	//weight ^ Generality
	static double weightExpoDepth(KB *, TaxoPattern *, TaxoPattern *);

	//Return the intersection as a depthVector
	static depthVector dVector(KB *, TaxoPattern *, TaxoPattern *);

	//Jaccard version of the dVector
	static depthVector dVectorJaccard(KB*, TaxoPattern *, TaxoPattern *);
};

#endif
