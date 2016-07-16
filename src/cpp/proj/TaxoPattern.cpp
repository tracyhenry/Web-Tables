#include "TaxoPattern.h"
#include <vector>
#include <algorithm>
#include <unordered_map>
#define IterID unordered_map<int, double>::iterator
#define IterIT unordered_map<int, TaxoPattern *>::iterator
using namespace std;

TaxoPattern::TaxoPattern()
{
	c.clear(), e.clear(); numEntity = 0;
}

void TaxoPattern::add(TaxoPattern *o)
{
	//merge concepts
	unordered_map<int, double> &oConceptMap = o->c;
	for (auto kv : oConceptMap)
		c[kv.first] += kv.second;

	//merge entities
	unordered_map<int, double> &oEntityMap = o->e;
	for (auto kv : oEntityMap)
		e[kv.first] = kv.second;

	//add numentity
	numEntity += o->numEntity;
}

int depthVector::operator < (const depthVector &o) const
{
/*	for (int i = (int) w.size() - 1; i >= 0; i --)
	{
		if (w[i] > o.w[i] + 1e-9)
			return 1;
		if (w[i] + 1e-9 < o.w[i])
			return 0;
	}

	double sum0 = 0, sum1 = 0;
	for (double x : w)
		sum0 += x;
	for (double x : o.w)
		sum1 += x;
	return sum0 > sum1;
*/
	return score(5) > o.score(5);
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

depthVector depthVector::add(depthVector o)
{
	depthVector ans((int) w.size());
	for (int i = 0; i < (int) ans.w.size(); i ++)
		ans.w[i] = w[i] + o.w[i];

	return ans;
}

void depthVector::addUpdate(depthVector o)
{
	for (int i = 0; i < (int) w.size(); i ++)
		w[i] += o.w[i];
}

depthVector depthVector::mAx(depthVector o)
{
	depthVector ans((int) w.size());
	for (int i = 0; i < (int) ans.w.size(); i ++)
		ans.w[i] = max(w[i], o.w[i]);

	return ans;
}

int depthVector::maxUpdate(depthVector o)
{
	if (o < (* this))
	{
		for (int i = 0; i < (int) w.size(); i ++)
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
	for (int i = 0; i < (int) w.size(); i ++)
		w[i] /= f;
}

double depthVector::score(double coef) const
{
	double ans = 0;
	for (int i = 0; i < (int) w.size(); i ++)
		ans += w[i] * exp(log(coef) * i);

	return ans;
}
