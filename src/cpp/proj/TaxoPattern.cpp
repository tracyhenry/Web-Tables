#include "TaxoPattern.h"
#include <vector>
#include <algorithm>
#include <unordered_map>
#define IterID unordered_map<int, double>::iterator
#define IterIT unordered_map<int, TaxoPattern *>::iterator
using namespace std;

TaxoPattern::TaxoPattern()
{
	c.clear(), e.clear();
}

void TaxoPattern::add(TaxoPattern *o)
{
	//merge concepts
	unordered_map<int, double> &oConceptMap = o->c;
	for (IterID it = oConceptMap.begin();
		it != oConceptMap.end(); it ++)
		c[it->first] += it->second;

	//merge entities
	unordered_map<int, double> &oEntityMap = o->e;
	for (IterID it = oEntityMap.begin();
		it != oEntityMap.end(); it ++)
		e[it->first] += it->second;
}

int depthVector::operator < (const depthVector &o) const
{
	for (int i = (int) w.size() - 1; i >= 0; i --)
	{
		if (w[i] > o.w[i] + 1e-9)
			return 1;
		if (w[i] + 1e-9 < o.w[i])
			return 0;
	}
	return 0;
}

depthVector::depthVector()
{
	w.clear();
}

depthVector::depthVector(int totalDepth)
{
	w.clear();
	w.resize(totalDepth);
}

depthVector depthVector::add(depthVector &o)
{
	depthVector ans((int) w.size());
	for (int i = 0; i < ans.w.size(); i ++)
		ans.w[i] = w[i] + o.w[i];

	return ans;
}

void depthVector::addUpdate(depthVector &o)
{
	for (int i = 0; i < w.size(); i ++)
		w[i] += o.w[i];
}

depthVector depthVector::mAx(depthVector &o)
{
	depthVector ans((int) w.size());
	for (int i = 0; i < ans.w.size(); i ++)
		ans.w[i] = max(w[i], o.w[i]);

	return ans;
}

int depthVector::maxUpdate(depthVector &o)
{
	if (o < (* this))
	{
		for (int i = 0; i < w.size(); i ++)
			w[i] = o.w[i];
		//updated
		return 1;
	}
	//not updated
	return 0;
}

void depthVector::normalize(double f)
{
	if (fabs(f) <= 1e-9)
		return ;
	for (int i = 0; i < w.size(); i ++)
		w[i] /= f;
}

double depthVector::score(double coef)
{
	double ans = 0;
	for (int i = 0; i < w.size(); i ++)
		ans += w[i] * exp(log(coef) * i);

	return ans;
}
