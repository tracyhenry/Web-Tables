#include "Matcher.h"
#include <set>
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;


double Matcher::weightedJaccard(KB *kb, TaxoPattern *cell, TaxoPattern *property)
{
	if (cell == NULL || property == NULL)
		return 0;
	//two sets
	unordered_map<int, int> &setA = cell->w;
	unordered_map<int, int> &setB = property->w;

	//union and intersect
	int commonWeight = 0;
	int unionWeight = 1e-9;

	//union setA
	for (unordered_map<int, int>::iterator it2 = setA.begin();
		it2 != setA.end(); it2 ++)
		{
			if (kb->getSucCount(it2->first))
				continue;
			if (! setB.count(it2->first))
				unionWeight += it2->second;
			else
				unionWeight += max(it2->second, setB[it2->second]);
		}

	//union setB
	for (unordered_map<int, int>::iterator it2 = setB.begin();
		it2 != setB.end(); it2 ++)
		{
			if (kb->getSucCount(it2->first))
				continue;
			if (! setA.count(it2->first))
				unionWeight += it2->second;
		}

	//intersect
	for (unordered_map<int, int>::iterator it2 = setA.begin();
		it2 != setA.end(); it2 ++)
		{
			if (kb->getSucCount(it2->first))
				continue;
			if (setB.count(it2->first))
				commonWeight += min(it2->second, setB[it2->first]);
		}

	return (double) commonWeight / unionWeight;
}

double Matcher::dotProduct(KB *kb, TaxoPattern *cell, TaxoPattern *property)
{
	if (cell == NULL || property == NULL)
		return 0;
	//two sets
	unordered_map<int, int> &setA = cell->w;
	unordered_map<int, int> &setB = property->w;

	double sim = 0;
	for (unordered_map<int, int>::iterator it = setA.begin();
		it != setA.end(); it ++)
		if (setB.count(it->first))
			sim += (it->second) * setB[it->first];

	return sim;
}

double Matcher::expoDepth(KB *kb, TaxoPattern *cell, TaxoPattern *property)
{
	if (cell == NULL || property == NULL)
		return 0;
	//two sets
	unordered_map<int, int> &setA = cell->w;
	unordered_map<int, int> &setB = property->w;

	double sim = 0;
	double coef = 10.0;

	for (unordered_map<int, int>::iterator it = setA.begin();
		it != setA.end(); it ++)
		if (setB.count(it->first))
			sim += (double) (it->second) * (double) setB[it->first]
				 * exp(log(coef) * kb->getDepth(it->first));

	return sim;
}
