#ifndef PARAM_H
#define PARAM_H

enum Similarity {DotProduct, Jaccard, Dice, wJaccard};

class Param
{
public:
	//weighting factor
	static double M;

	//normalized similarity or not
	static bool normalized;

	//the similarity measure used for column concept & relationship
	static Similarity colConceptSim;

	//the similarity measure used for record concept
	static Similarity recConceptSim;

	//The weight for semi-lucky cells
	static double WT_SEMILUCKY;

	//The threshold for the depth of a candidate concept
	static double TH_DEPTH;

	//The maximum threshold for the proportion of lucky cells
	static double TMAX;

	//The minimum threshold for the proportion of lucky cells
	static double TMIN;

	//exponential base factor used to punish long distances
	static double DISEXPBASE;

	//max k for record type pair enrichment
	static int MAXK;

	//limitation on number of type pair enrichments
	static int TYPE_PAIR_K;

	//set to default values
	static void setToDefault();
};

#endif
