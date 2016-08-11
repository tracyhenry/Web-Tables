#ifndef PARAM_H
#define PARAM_H

enum Similarity {DotProduct, Jaccard, Dice, wJaccard};

class Param
{
public:
	//weighting factors for stratified similarities
	static double M_DotProduct;
	static double M_Jaccard;
	static double M_Dice;

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

	//
	static double DISEXPBASE;

	//set to default values
	void setToDefault();
};

#endif