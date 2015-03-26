#ifndef TAXO_PATTERN
#define TAXO_PATTERN

struct TaxoPattern
{
	TaxoPattern() {w.clear()}
	//From concept to weight
	unordered_map<int, int> w;
};

#endif
