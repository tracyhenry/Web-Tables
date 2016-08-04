#include "Matcher.h"
#include <set>
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

double Matcher::M = 5.0;

double Matcher::patternSim(KB *kb, TaxoPattern *p1, TaxoPattern *p2)
{
	depthVector dv = dVector(kb, p1, p2);
	return dv.score(M);
}

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

depthVector Matcher::dVector(KB *kb, TaxoPattern *p1, TaxoPattern *p2)
{
	int H = kb->getDepth(kb->getRoot());
	depthVector ans(H + 1);

	double w1 = p1->numEntity;
	double w2 = p2->numEntity;
	if (p1 == NULL || p2 == NULL || fabs(w1) < 1e-9 || fabs(w2) < 1e-9)
		return ans;

	unordered_map<int, double> &c1 = p1->c;
	unordered_map<int, double> &c2 = p2->c;
	unordered_map<int, double> &e1 = p1->e;
	unordered_map<int, double> &e2 = p2->e;
	//Concept set
	for (auto kv : c1)
		if (c2.count(kv.first))
			ans.w[H - kb->getDepth(kv.first)] += kv.second * c2[kv.first] / w1 / w2;
	//Entity set
	for (auto kv : e1)
		if (e2.count(kv.first))
			ans.w[H] += kv.second * e2[kv.first] / w1 / w2;

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

	unordered_map<int, double> &c1 = p1->c;
	unordered_map<int, double> &c2 = p2->c;
	unordered_map<int, double> &e1 = p1->e;
	unordered_map<int, double> &e2 = p2->e;
	vector<double> iWeight(H + 1), uWeight(H + 1);
	//concepts
	for (auto kv : c1)
	{
		int level = H - kb->getDepth(kv.first);
		//union
		double cur = (c2.count(kv.first) ? c2[kv.first] / w2 : 0);
		cur = max(cur, kv.second / w1);
		uWeight[level] += cur;
		//intersection
		if (c2.count(kv.first))
			iWeight[level] += min(kv.second / w1, c2[kv.first] / w2);
	}
	for (auto kv : c2)
	{
		int level = H - kb->getDepth(kv.first);
		if (c1.count(kv.first))
			continue;
		uWeight[level] += kv.second / w2;
	}
	//entities
	for (auto kv : e1)
	{
		//union
		double cur = (e2.count(kv.first) ? e2[kv.first] / w2 : 0);
		cur = max(cur, kv.second / w1);
		uWeight[H] += cur;
		//intersection
		if (e2.count(kv.first))
			iWeight[H] += min(kv.second / w1, e2[kv.first] / w2);
	}
    for (auto kv : e2)
	{
		if (e1.count(kv.first))
			continue;
		uWeight[H] += kv.second / w2;
	}
	//compute ans
	for (int h = H; h >= 0; h --)
		ans.w[h] = (fabs(uWeight[h]) >= 1e-9 ? iWeight[h] / uWeight[h] : 0);
	return ans;
}

depthVector Matcher::dVectorDiff(KB *kb, TaxoPattern *p1, TaxoPattern *p2)
{
	int H = kb->getDepth(kb->getRoot());
	depthVector ans(H + 1);

	double w1 = p1->numEntity;
	double w2 = p2->numEntity;
	if (p1 == NULL || p2 == NULL || fabs(w1) < 1e-9 || fabs(w2) < 1e-9)
		return ans;

	unordered_map<int, double> &c1 = p1->c;
	unordered_map<int, double> &c2 = p2->c;
	unordered_map<int, double> &e1 = p1->e;
	unordered_map<int, double> &e2 = p2->e;
	//concepts
	for (auto kv : c1)
	{
		if (! c2.count(kv.first))
			continue;
		int level = H - kb->getDepth(kv.first);
		double diff = (c2.count(kv.first) ? c2[kv.first] / w2 : 0);
		diff = 1.0 - abs(diff - kv.second / w1);
		ans.w[level] += diff;
	}
/*	for (auto kv : c2)
	{
		int level = H - kb->getDepth(kv.first);
		if (c1.count(kv.first))
			continue;
		ans.w[level] += 1.0 - kv.second / w2;
	}
*/	//entities
	for (auto kv : e1)
	{
		if (! e2.count(kv.first))
			continue;
		double diff = (e2.count(kv.first) ? e2[kv.first] / w2 : 0);
		diff = 1.0 - abs(diff - kv.second / w1);
		ans.w[H] += diff;
	}
/*	for (auto kv : e2)
	{
		if (e1.count(kv.first))
			continue;
		ans.w[H] += 1.0 - kv.second / w2;
	}
*/
	return ans;
}
