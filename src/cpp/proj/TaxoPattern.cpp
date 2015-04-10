#include "TaxoPattern.h"
#include <vector>
#include <algorithm>
using namespace std;

TaxoPattern::TaxoPattern()
{
	c.clear(), e.clear();
}

depthVector::depthVector()
{
	w.clear();
}

depthVector::depthVector(int totalDepth)
{
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

depthVector depthVector::max(depthVector &o)
{
	depthVector ans((int) w.size());
	for (int i = 0; i < ans.w.size(); i ++)
		ans.w[i] = max(w[i], o.w[i]);

	return ans;
}

void depthVector::maxUpdate(depthVector &o)
{
	for (int i = 0; i < w.size(); i ++)
		w[i] = max(w[i], o.w[i]);
}

double depthVector::score(double coef)
{
	double ans = 0;
	for (int i = 0; i < w.size(); i ++)
		ans += w[i] * exp(log(coef) * i);

	return ans;
}
