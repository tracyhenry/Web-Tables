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
	unordered_map<int, int> &setA = cell->c;
	unordered_map<int, int> &setB = property->c;

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
	unordered_map<int, int> &setA = cell->c;
	unordered_map<int, int> &setB = property->c;

	double sim = 0;
	for (unordered_map<int, int>::iterator it = setA.begin();
		it != setA.end(); it ++)
		if (setB.count(it->first))
			sim += (it->second) * setB[it->first];

	return sim;
}

double Matcher::constExpoDepth(KB *kb, TaxoPattern *cell, TaxoPattern *property)
{
	if (cell == NULL || property == NULL)
		return 0;

	double sim = 0;
	double coef = 1000.0;
	int H = kb->getDepth(kb->getRoot());

	//Concept set
	unordered_map<int, int> &cA = cell->c;
	unordered_map<int, int> &cB = property->c;

	for (unordered_map<int, int>::iterator it = cA.begin();
		it != cA.end(); it ++)
		if (cB.count(it->first))
			sim += (double) (it->second) * (double) cB[it->first]
				 * exp(log(coef) * (H - kb->getDepth(it->first)));

	//Entity set
	unordered_map<int, int> &eA = cell->e;
	unordered_map<int, int> &eB = property->e;

	for (unordered_map<int, int>::iterator it = eA.begin();
		it != eA.end(); it ++)
		if (eB.count(it->first))
			sim += (double) (it->second) * (double) eB[it->first]
				* exp(log(coef) * H);
	return sim;
}

double Matcher::weightExpoDepth(KB *kb, TaxoPattern *cell, TaxoPattern *property)
{
	if (cell == NULL || property == NULL)
		return 0;
	double sim = 0;
	int H = kb->getDepth(kb->getRoot());

	//Concept set
	unordered_map<int, int> &cA = cell->c;
	unordered_map<int, int> &cB = property->c;

	for (unordered_map<int, int>::iterator it = cA.begin();
		it!= cA.end(); it ++)
		if (cB.count(it->first))
			sim += exp(log(it->second * cB[it->first]) *
				(H - kb->getDepth(it->first)));

	//Entity set
	unordered_map<int, int> &eA = cell->e;
	unordered_map<int, int> &eB = property->e;

	for (unordered_map<int, int>::iterator it = eA.begin();
		it != eA.end(); it ++)
		if (eB.count(it->first))
			sim += exp(log(it->second * eB[it->first]) * H);
	return sim;
}

