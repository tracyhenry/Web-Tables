#ifndef TAXO_PATTERN
#define TAXO_PATTERN

#include <unordered_map>
#include <vector>

struct TaxoPattern
{
	TaxoPattern();

	//From concept to weight
	std::unordered_map<int, int> c;

	//From entity to weight
	std::unordered_map<int, int> e;
};

struct depthVector
{
	//comparison
	int operator <(const depthVector &) const;

	//constructor
	depthVector();
	depthVector(int);

	//add
	depthVector add(depthVector &);
	void addUpdate(depthVector &);

	//max
	depthVector mAx(depthVector &);
	void maxUpdate(depthVector &);

	//convert to exponential score
	double score(double);

	//From depth to weight
	std::vector<double> w;
};

#endif
