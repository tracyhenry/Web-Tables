#include "Param.h"
using namespace std;

double Param::M_DotProduct = 5.0;
double Param::M_Jaccard = 10.0;
double Param::M_Dice = 5.0;
bool Param::normalized = false;
Similarity Param::colConceptSim = Jaccard;
Similarity Param::recConceptSim = DotProduct;
double Param::WT_SEMILUCKY = 0.25;
double Param::TH_DEPTH = 12;
double Param::TMAX = 0.8;
double Param::TMIN = 0.4;
double Param::DISEXPBASE = 10.0;

void Param::setToDefault()
{
	M_DotProduct = 5.0;
	M_Jaccard = 10.0;
	M_Dice = 5.0;
	normalized = false;
	colConceptSim = Jaccard;
	recConceptSim = DotProduct;
	WT_SEMILUCKY = 0.25;
	TH_DEPTH = 12;
	TMAX = 0.8;
	TMIN = 0.4;
	DISEXPBASE = 10.0;
}
