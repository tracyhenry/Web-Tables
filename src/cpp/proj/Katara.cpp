#include "Bridge.h"
#include "KB.h"
#include "Matcher.h"
#include <cstring>
#include <algorithm>
#include <unordered_set>
#include <iostream>
using namespace std;

/**
struct KataraListEntry
{
	std::string type;
	int x, y;
	std::vector<std::pair<double, int>> candidates;

	KataraListEntry() {}
	KataraListEntry(std::string t) : type(t) {}
};
*/

void Bridge::initRankedLists(int tid)
{
	Table curTable = corpus->getTableByDataId(tid);
	int nRow = curTable.nRow;
	int nCol = curTable.nCol;
	rankedLists.clear();

	//individual columns
	for (int i = 0; i < nCol; i ++)
	{
		rankedLists.push_back(KataraListEntry("col", i, -1));
		double numLuckyCell = getNumLuckyCells(curTable, i);
		double luckyRate = (double) numLuckyCell / nRow;
		double threshold = TMIN + (TMAX - TMIN) * luckyRate;
		for (auto kv : colPattern[curTable.id][i]->c)
		{
			int c = kv.first;
			double numContainedCell = getNumContainedCells(curTable, i, c);
			if (numContainedCell / numLuckyCell < threshold)
				continue;
			double tfIdf = 0;
			for (int j = 0; j < nRow; j ++)
			{
				int cellId = curTable.cells[j][i].id;
				//tf
				bool belong = false;
				for (auto kv : matches[cellId])
					if (kb->checkRecursiveBelong(kv.first, c))
					{
						belong = true; break;
					}
				double tf = (belong ? 1.0 / log(kb->getRecursivePossessCount(c)) : 0);
				//idf
				double idf = 0;
				for (auto kv : matches[cellId])
					idf += kb->getBelongCount(kv.first);
				idf += 1e-9;
				idf = log(kb->countConcept() / idf);
				tfIdf += tf * idf;
			}
			if (fabs(tfIdf) >= 1e-9)
				rankedLists[i].candidates.emplace_back(tfIdf, c);
		}
		//sort and normalize
		sort(rankedLists[i].candidates.begin(), rankedLists[i].candidates.end());
		reverse(rankedLists[i].candidates.begin(), rankedLists[i].candidates.end());
		double maxs = 0;
		for (auto kv : rankedLists[i].candidates)
			maxs = max(maxs, kv.first);
		for (auto& kv : rankedLists[i].candidates)
			kv.first /= maxs;
	}

	for (int i = 0; i < nCol; i ++)
		for (int j = i + 1; j < nCol; j ++)
		{
			rankedLists.push_back(KataraListEntry("rel", i, j));
			int idx = (int) rankedLists.size() - 1;
			for (int rel = 1; rel <= kb->countRelation(); rel ++)
			{
				double tfIdf = 0;
				for (int t = 0; t < nRow; t ++)
				{
					int c1 = curTable.cells[t][i].id;
					int c2 = curTable.cells[t][j].id;
					unordered_set<int> ent1, ent2;
					for (auto kv : matches[c1])
						ent1.insert(kv.first);
					for (auto kv : matches[c2])
						ent2.insert(kv.first);
					//tf
					bool hasRel = false;
					for (int e1 :ent1)
					{
						int factCount = kb->getFactCount(e1);
						for (int k = 0; k < factCount; k ++)
						{
							pair<int, int> cp = kb->getFactPair(e1, k);
							if (cp.first == rel && ent2.count(cp.second))
							{
								hasRel = true;
								break;
							}
						}
						if (hasRel)
							break;
					}
					double tf = (hasRel ? 1.0 / log(kb->getRelTripleCount(rel)) : 0);
					//idf
					double idf = 0;
					for (int e1 : ent1)
						for (int e2 : ent2)
							idf += kb->getEntPairTripleCount(e1, e2);
					idf += 1e-9;
					idf = log(kb->countRelation() / idf);
					tfIdf += tf * idf;
				}
				if (fabs(tfIdf) >= 1e-9)
					rankedLists[idx].candidates.emplace_back(tfIdf, rel);
			}
			//sort and normalize
			sort(rankedLists[idx].candidates.begin(), rankedLists[idx].candidates.end());
			reverse(rankedLists[idx].candidates.begin(), rankedLists[idx].candidates.end());
			double maxs = 0;
			for (auto kv : rankedLists[idx].candidates)
				maxs = max(maxs, kv.first);
			for (auto& kv : rankedLists[idx].candidates)
				kv.first /= maxs;
		}
}

void Bridge::initCoherenceScores()
{
	int totalConcept = kb->countConcept();
	int totalRelation = kb->countRelation();
	int totalEntity = kb->countEntity();

	//make entRels
	entRels.clear(), entRels.resize(totalEntity + 1);
	for (int i = 1; i <= totalEntity; i ++)
	{
		int factCount = kb->getFactCount(i);
		for(int j = 0; j < factCount; j ++)
		{
			pair<int, int> cur = kb->getFactPair(i, j);
			entRels[i].insert(cur.first);
		}
	}

	//compute Pr(T)
	p1.clear(), p1.resize(totalConcept + 1);
	for (int i = 1; i <= totalConcept; i ++)
	{
		p1[i] = (double) kb->getRecursivePossessCount(i);
		p1[i] /= (double) totalEntity;
		p1[i] += 1e-9;
	}

	//compute Pr_sub(P)
	p2.clear(), p2.resize(totalRelation + 1);
	for (int i = 1; i <= totalEntity; i ++)
		for (int rel : entRels[i])
			p2[rel] += 1.0;
	for (int i = 1; i <= totalRelation; i ++)
		p2[i] /= (double) totalEntity, p2[i] += 1e-9;

	//compute Pr_sub(P intersect T) using DFS
	p3.clear(), p3.resize(totalConcept + 1);
	for (int i = 1; i <= totalConcept; i ++)
		p3[i].resize(totalRelation + 1);
	kataraDfs(kb->getRoot());
	for (int i = 1; i <= totalConcept; i ++)
		for (int j = 1; j <= totalRelation; j ++)
			p3[i][j] /= (double) totalEntity, p3[i][j] += 1e-9;

	//compute coherence score
	relSC.clear(), relSC.resize(totalConcept + 1);
	for (int i = 1; i <= totalConcept; i ++)
	{
		relSC[i].resize(totalRelation + 1);
		for (int j = 1; j <= totalRelation; j ++)
		{
			relSC[i][j] = log(p3[i][j] / p1[i] / p2[j]);
			relSC[i][j] /= -log(p3[i][j]);
			relSC[i][j] = (relSC[i][j] + 1) / 2.0;
		}
	}
}

void Bridge::kataraDfs(int x)
{
	int totalChild = kb->getSucCount(x);
	int directPossessCount = kb->getPossessCount(x);
	//my own entities
	for (int i = 0; i < directPossessCount; i ++)
	{
		int e = kb->getPossessEntity(x, i);
		for (int rel : entRels[e])
			p3[x][rel] += 1.0;
	}

	//recursion
	for (int i = 0; i < totalChild; i ++)
	{
		int j = kb->getSucNode(x, i);
		kataraDfs(j);
		for (int rel = 1; rel < (int) p3[x].size(); rel ++)
			p3[x][rel] += p3[j][rel];
	}
}

vector<int> Bridge::kataraFindColConceptAndRelation(int tid, bool print)
{
	Table curTable = corpus->getTableByDataId(tid);
	int nCol = curTable.nCol;
	int entityCol = curTable.entityCol;

	initRankedLists(tid);
	//make upper bounds
	ubs.clear(), ubs.resize(rankedLists.size());
	sumUbs.clear(), sumUbs.resize(rankedLists.size());
	for(int i = (int) ubs.size() - 1; i >= 0; i --)
		if (rankedLists[i].type == "rel")
		{
			ubs[i] = 0;
			int x = rankedLists[i].x;
			int y = rankedLists[i].y;
			for (auto kv : rankedLists[i].candidates)
			{
				double maxv;
				double score = kv.first;
				int rel = kv.second;
				int reverseRel = kb->getReverseRelationId(rel);
				//find max for x
				maxv = 0;
				for (auto it : rankedLists[x].candidates)
					maxv = max(maxv, relSC[it.second][rel]);
				score += maxv;
				//find max for y
				maxv = 0;
				for (auto it : rankedLists[y].candidates)
					maxv = max(maxv, relSC[it.second][reverseRel]);
				score += maxv;
				ubs[i] = max(ubs[i], score);
			}
			sumUbs[i] = ubs[i];
			if (i + 1 < (int) sumUbs.size())
				sumUbs[i] += sumUbs[i + 1];
		}
		else
		{
			ubs[i] = 0;
			if (rankedLists[i].candidates.size())
				ubs[i] = rankedLists[i].candidates[0].first;
			sumUbs[i] = ubs[i];
			if (i + 1 < (int) sumUbs.size())
				sumUbs[i] += sumUbs[i + 1];
		}

	//backtrace
	maxScore = 0;
	curState.clear(), curState.resize(rankedLists.size());
	ansState.clear(), ansState.resize(rankedLists.size());
	for (int i = 0; i < (int) rankedLists.size(); i ++)
		curState[i] = ansState[i] = -1;
	kataraBackTrace(0, nCol, 0.0);

	//col
	vector<int> ans(nCol * 2, -1);
	for (int i = 0; i < nCol; i ++)
		ans[i] = ansState[i];
	//rel
	for (int i = 0; i < nCol; i ++)
	{
		if (i == entityCol)
			continue;
		int x = min(i, entityCol);
		int y = max(i, entityCol);
		for (int j = nCol; j < (int) rankedLists.size(); j ++)
			if (rankedLists[j].x == x && rankedLists[j].y == y)
				ans[i + nCol] = ansState[j];
	}
	if (print)
	{
		cout << endl << "Entity Column : " << entityCol << endl << endl;
		for (int i = 0; i < nCol; i ++)
			cout << "Column " << i << " : " << endl
				<< '\t' << (ans[i] == -1 ? "No Concept" : kb->getConcept(ans[i])) << endl
				<< '\t' << (ans[i + nCol] == -1 ? "No Relation" : kb->getRelation(ans[i + nCol])) << endl;
		cout <<endl;
	}
	return ans;
}

void Bridge::kataraBackTrace(int x, int nCol, double s)
{
	if (x == nCol)
	{
		//add rel scores to s
		for (int i = nCol; i < (int) rankedLists.size(); i ++)
		{
			double maxv = 0;
			int x = rankedLists[i].x;
			int y = rankedLists[i].y;
			curState[i] = -1;
			for (auto kv : rankedLists[i].candidates)
			{
				double score = kv.first;
				int rel = kv.second;
/*				int reverseRel = kb->getReverseRelationId(rel);
				if (curState[x] != -1)
					score += relSC[curState[x]][rel];
				if (curState[y] != -1)
					score += relSC[curState[y]][reverseRel];
*/				if (score > maxv)
					maxv = score, curState[i] = rel;
			}
			s += maxv;
		}
		if (s > maxScore)
			maxScore = s, ansState = curState;
		return ;
	}
	//prune
//	if (s + sumUbs[x] <= maxScore)
//		return ;
	for (auto kv : rankedLists[x].candidates)
	{
		curState[x] = kv.second;
		kataraBackTrace(x + 1, nCol, s + kv.first);
		curState[x] = -1;
	}
	kataraBackTrace(x + 1, nCol, s);
}
