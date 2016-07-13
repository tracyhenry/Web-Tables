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
	unordered_map<int, double> &setA = cell->c;
	unordered_map<int, double> &setB = property->c;

	//union and intersect
	double intersectWeight = 0;
	double unionWeight = 1e-9;

	//union setA
	for (unordered_map<int, double>::iterator it2 = setA.begin();
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
	for (unordered_map<int, double>::iterator it2 = setB.begin();
		it2 != setB.end(); it2 ++)
		{
			if (kb->getSucCount(it2->first))
				continue;
			if (! setA.count(it2->first))
				unionWeight += it2->second;
		}

	//intersect
	for (unordered_map<int, double>::iterator it2 = setA.begin();
		it2 != setA.end(); it2 ++)
		{
			if (kb->getSucCount(it2->first))
				continue;
			if (setB.count(it2->first))
				intersectWeight += min(it2->second, setB[it2->first]);
		}

	return (double) intersectWeight / unionWeight;
}

double Matcher::dotProduct(KB *kb, TaxoPattern *cell, TaxoPattern *property)
{
	if (cell == NULL || property == NULL)
		return 0;
	//two sets
	unordered_map<int, double> &setA = cell->c;
	unordered_map<int, double> &setB = property->c;

	double sim = 0;
	for (unordered_map<int, double>::iterator it = setA.begin();
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
	unordered_map<int, double> &cA = cell->c;
	unordered_map<int, double> &cB = property->c;

	for (unordered_map<int, double>::iterator it = cA.begin();
		it != cA.end(); it ++)
		if (cB.count(it->first))
			sim += (double) (it->second) * (double) cB[it->first]
				 * exp(log(coef) * (H - kb->getDepth(it->first)));

	//Entity set
	unordered_map<int, double> &eA = cell->e;
	unordered_map<int, double> &eB = property->e;

	for (unordered_map<int, double>::iterator it = eA.begin();
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
	unordered_map<int, double> &cA = cell->c;
	unordered_map<int, double> &cB = property->c;

	for (unordered_map<int, double>::iterator it = cA.begin();
		it!= cA.end(); it ++)
		if (cB.count(it->first))
			sim += exp(log(it->second * cB[it->first]) *
				(H - kb->getDepth(it->first)));

	//Entity set
	unordered_map<int, double> &eA = cell->e;
	unordered_map<int, double> &eB = property->e;

	for (unordered_map<int, double>::iterator it = eA.begin();
		it != eA.end(); it ++)
		if (eB.count(it->first))
			sim += exp(log(it->second * eB[it->first]) * H);
	return sim;
}

depthVector Matcher::dVector(KB *kb, TaxoPattern *cell, TaxoPattern *property)
{
	int H = kb->getDepth(kb->getRoot());
	depthVector ans(H + 1);

	if (cell == NULL || property == NULL)
		return ans;

	//Concept set
	unordered_map<int, double> &cA = cell->c;
	unordered_map<int, double> &cB = property->c;

	for (unordered_map<int, double>::iterator it = cA.begin();
		it != cA.end(); it ++)
		if (cB.count(it->first))
			ans.w[H - kb->getDepth(it->first)] += (double) it->second * cB[it->first];

	//Entity set
	unordered_map<int, double> &eA = cell->e;
	unordered_map<int, double> &eB = property->e;

	for (unordered_map<int, double>::iterator it = eA.begin();
		it != eA.end(); it ++)
		if (eB.count(it->first))
			ans.w[H] += (double) it->second * eB[it->first];

	return ans;
}

depthVector Matcher::dVectorJaccard(KB *kb, TaxoPattern *p1, TaxoPattern *p2)
{
	int H = kb->getDepth(kb->getRoot());
	double w1 = p1->numEntity;
	double w2 = p2->numEntity;
	depthVector ans(H + 1);

	if (p1 == NULL || p2 == NULL || fabs(w1) < 1e-9 || fabs(w2) < 1e-9)
		return ans;

	//a map mapping from depth to set
	vector<unordered_map<int, double>> M1(H + 1), M2(H + 1);

	//concept set
	for (auto kv : p1->c)
		M1[H - kb->getDepth(kv.first)][kv.first] += kv.second / w1;
	for (auto kv : p2->c)
		M2[H - kb->getDepth(kv.first)][kv.first] += kv.second / w2;

	//entity set
	for (auto kv : p1->e)
		M1[H][kv.first] += kv.second / w1;
	for (auto kv : p2->e)
		M2[H][kv.first] += kv.second / w2;

	//compute ans
	for (int h = H; h >= 0; h --)
	{
		double unionWeight = 0, intersectWeight = 0;
		//union
		for (auto kv : M1[h])
			unionWeight += max(kv.second, (M2[h].count(kv.first) ? M2[h][kv.first] : 0));
		for (auto kv : M2[h])
			if (! M1[h].count(kv.first))
				unionWeight += kv.second;
		//intersect
		for (auto kv : M1[h])
			if (M2[h].count(kv.first))
				intersectWeight += min(kv.second, M2[h][kv.first]);
		ans.w[h] = (fabs(unionWeight) >= 1e-9 ? intersectWeight / unionWeight : 0);
	}
	return ans;
}
