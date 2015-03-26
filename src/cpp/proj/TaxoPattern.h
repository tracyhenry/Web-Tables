#ifndef TAXO_PATTERN
#define TAXO_PATTERN

#include <unordered_map>

struct TaxoPattern
{
	TaxoPattern() {w.clear();}
	//From concept to weight
	std::unordered_map<int, int> w;
};

#endif
