#ifndef TAXO_PATTERN
#define TAXO_PATTERN

#include <unordered_map>

struct TaxoPattern
{
	TaxoPattern() {c.clear(), e.clear();}
	//From concept to weight
	std::unordered_map<int, int> c;

	//From entity to weight
	std::unordered_map<int, int> e;
};

#endif
