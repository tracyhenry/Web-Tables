#include "Param.h"
using namespace std;

//not for experiments
bool Param::normalized = false;
double Param::WT_SEMILUCKY = 0.25;
double Param::TH_DEPTH = 12;
int Param::MAXK = 5;
int Param::TYPE_PAIR_K = 10000;

//experiments
double Param::M = 5.0;
double Param::DISEXPBASE = 10.0;
Similarity Param::colConceptSim = Jaccard;
Similarity Param::recConceptSim = DotProduct;
double Param::TMAX = 0.8;
double Param::TMIN = 0.4;


void Param::setToDefault()
{
	normalized = false;
	WT_SEMILUCKY = 0.25;
	TH_DEPTH = 12;
	MAXK = 5;
	TYPE_PAIR_K = 10000;

	M = 5.0;
	DISEXPBASE = 10.0;
	colConceptSim = Jaccard;
	recConceptSim = DotProduct;
	TMAX = 0.8;
	TMIN = 0.4;
}
