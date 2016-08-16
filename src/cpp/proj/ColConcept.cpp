#include "Bridge.h"
#include "Matcher.h"
#include <cstring>
#include <algorithm>
#include <unordered_set>
#include <iostream>
using namespace std;

double Bridge::getNumLuckyCells(Table *curTable, int c)
{
	double numLuckyCell = 0;
	for (int i = 0; i < curTable->nRow; i ++)
		if (matches[curTable->cells[i][c].id].size())
		{
			if (matches[curTable->cells[i][c].id][0].second == 1.0)
				numLuckyCell += 1.0;
			else
				numLuckyCell += Param::WT_SEMILUCKY;
		}
	return numLuckyCell;
}

double Bridge::getNumContainedCells(Table *curTable, int c, int conceptId)
{
	double numContainedCell = 0;
	for (int j = 0; j < curTable->nRow; j ++)
	{
		vector<pair<int, double>> curMatches = matches[curTable->cells[j][c].id];
		if (curMatches.empty())
			continue;
		for (auto kv: curMatches)
		{
			int entityId = kv.first;
			if (kb->checkRecursiveBelong(entityId, conceptId))
			{
				numContainedCell += (curMatches[0].second == 1.0 ? 1.0 : Param::WT_SEMILUCKY);
				break;
			}
		}
	}
	return numContainedCell;
}

double Bridge::getThreshold(Table *curTable, int c)
{
	//hashId
	string hashId = to_string(curTable->table_id) + "#" + to_string(c);

	//check cache
	if (thCache.count(hashId))
		return thCache[hashId];

	//calculate
	double numLuckyCell = getNumLuckyCells(curTable, c);
	double luckyRate = numLuckyCell / curTable->nRow;
	return thCache[hashId] = Param::TMIN + (Param::TMAX - Param::TMIN) * luckyRate;
}

/**
 * Given a table_id and column number,
 * output top-k concepts describing this column.
 * This is the Naive Majority method.
 */

vector<int> Bridge::findColConceptMajority(int tid, int c, bool print)
{
	Table *curTable = corpus->getTableByDataId(tid);
	//check range
	if (c < 0 || c >= curTable->nCol)
	{
		cout << "Column index is out of range!" << "   " << tid << " " << c << endl;
		return vector<int>();
	}

	//Compute total lucky cell in this column
	double numLuckyCell = getNumLuckyCells(curTable, c);
	if (! numLuckyCell)
		return vector<int>();

	//Make candidate set from column pattern
	unordered_set<int> candidates;
	for (auto kv : colPattern[curTable->id][c]->c)
		candidates.insert(kv.first);

	//Loop over all concepts
	vector<pair<int, int>> score;
	for (int conceptId : candidates)
	{
		double numContainedCell = getNumContainedCells(curTable, c, conceptId);
		if (numContainedCell / numLuckyCell >= getThreshold(curTable, c))
			score.emplace_back(kb->getDepth(conceptId), conceptId);
	}
	//Sort by depth
	sort(score.begin(), score.end());

	//print
	if (print)
		for (int i = 0; i < (int) score.size(); i ++)
			cout << kb->getConcept(score[i].second) << " " << score[i].first << endl;

	vector<int> ans;
	for (int i = 0; i < (int) score.size(); i ++)
		ans.push_back(score[i].second);

	return ans;
}

vector<int> Bridge::baselineFindColConceptAndRelation(int tid, bool print)
{
	Table *curTable = corpus->getTableByDataId(tid);
	int nCol = curTable->nCol;
	vector<int> ans(nCol * 2, - 1);
	vector<int> ansColConcept;
	for (int i = 0; i < nCol; i ++)
	{
		vector<int> curAns = findColConceptMajority(tid, i, false);
		ans[i] = (curAns.size() ? curAns[0] : -1);
		ans[i + nCol] = naiveFindRelation(tid, i, false);
	}
        if (print)
        {
                cout << endl << "Entity Column : " << curTable->entityCol << endl << endl;
                for (int i = 0; i < nCol; i ++)
                        cout << "Column " << i << " : " << endl
                                << '\t' << (ans[i] == -1 ? "No Concept" : kb->getConcept(ans[i])) << endl
                                << '\t' << (ans[i + nCol] == -1 ? "No Relation" : kb->getRelation(ans[i + nCol])) << endl;
                cout <<endl;
        }
	return ans;
}

/**
* Given a table id,
* output a vector of int with the first nCol elements being
* the concept IDs and the last nCol elements being the
* relation IDs.
* For the entity column, the rel will be -1.
*/

vector<int> Bridge::findColConceptAndRelation(int tid, bool print)
{
	//current table
	Table *curTable = corpus->getTableByDataId(tid);
	int nCol = curTable->nCol;
	int numRelation = kb->countRelation();
	int entityCol = curTable->entityCol;
	vector<int> ans(nCol * 2, -1);
	double ansSim = 0;

	//cache lookup
	if (labelCache.count(tid))
	{
		vector<int> ans = labelCache[tid];
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

	//check if there is a given entity column
	if (entityCol == -1)
	{
		if (print)
			cout << "There is no entity column given! Quit..." << endl;
		return labelCache[tid] = ans;
	}

	//make a candidate concept set for each column
	vector<vector<int>> candidates(nCol);
	for (int i = 0; i < nCol; i ++)
	{
		for (auto kv : colPattern[curTable->id][i]->c)
		{
			int conceptId = kv.first;
			if (kv.second / colPattern[curTable->id][i]->numEntity >= getThreshold(curTable, i))
				if (kb->getDepth(conceptId) <= Param::TH_DEPTH)
					candidates[i].push_back(conceptId);
		}
//		if (candidates[i].empty())
//			candidates[i].push_back(kb->getRoot());
	}

	//calculate search space
	long long searchSpace = (long long) candidates[entityCol].size();
	long long sumCandSize = 0;
	for (int i = 0; i < nCol; i ++)
		if (i != entityCol)
			sumCandSize += (long long) candidates[i].size();
	searchSpace *= (long long) numRelation * (long long) (nCol - 1);
	searchSpace += (long long) sumCandSize * (long long) numRelation;
	if (print)
		cout << "Total search space : " << searchSpace << endl << endl;

	//share computation
	vector<vector<int>> attrConcepts(nCol, vector<int>(numRelation + 1, -1));
	vector<vector<double>> bestSims(nCol, vector<double>(numRelation + 1));
	for (int i = 0; i < nCol; i ++)
	{
		if (i == entityCol)
			continue;
		for (int rel = 1; rel <= numRelation; rel ++)
		{
			attrConcepts[i][rel] = -1;
			bestSims[i][rel] = 0;
			for (int conceptId : candidates[i])
			{
				if (! conSchema[conceptId].count(rel))
					continue;
				TaxoPattern *p1 = colPattern[curTable->id][entityCol];
				TaxoPattern *p2 = conSchema[conceptId][rel];
				double curSim = Matcher::patternSim(kb, p1, p2, Param::colConceptSim);
				if (curSim > bestSims[i][rel])
				{
					bestSims[i][rel] = curSim;
					attrConcepts[i][rel] = conceptId;
				}
			}
		}
	}

	for (int entityColConcept : candidates[entityCol])
	{
		double sumSim = 0;
		vector<int> curState(nCol * 2, -1);
		curState[entityCol] = entityColConcept;
		for (int i = 0; i < nCol; i ++)
		{
			if (i == entityCol) continue;
			double bestSim = 0;
			for (int rel = 1; rel <= numRelation; rel ++)
			{
				if (! conSchema[entityColConcept].count(rel))
					continue;
				int reverseRel = kb->getReverseRelationId(rel);
				int curConcept = attrConcepts[i][reverseRel];
				TaxoPattern *p1 = colPattern[curTable->id][i];
				TaxoPattern *p2 = conSchema[entityColConcept][rel];
				double curSim = Matcher::patternSim(kb, p1, p2, Param::colConceptSim);
				curSim += bestSims[i][reverseRel];
				if (curSim > bestSim)
				{
					bestSim = curSim;
					curState[i] = curConcept;
					curState[i + nCol] = rel;
				}
			}
			sumSim += bestSim;
		}
		if (sumSim > ansSim)
		{
			ansSim = sumSim;
			ans = curState;
		}
	}

	//special check
	bool noAns = true;
	for (int i = 0; i < nCol * 2; i ++)
		if (ans[i] != -1)
			noAns = false;
	if (noAns)
		for (int i = 0; i < nCol; i ++)
			if (candidates[i].size())
				ans[i] = candidates[i][0];
	//go down
	for (int i = 0; i < nCol; i ++)
	{
		if (ans[i] == -1)
			continue;
		unordered_set<int> s(candidates[i].begin(), candidates[i].end());
		while (1)
		{
			int sucCount = kb->getSucCount(ans[i]);
			int totalNext = 0;
			int next = -1;
			for (int j = 0; j < sucCount; j ++)
			{
				int ch = kb->getSucNode(ans[i], j);
				if (s.count(ch))
					totalNext ++, next = ch;
			}
			if (totalNext != 1)
				break;
			ans[i] = next;
		}
	}

	//print
	if (print)
	{
		cout << endl << "Entity Column : " << entityCol << endl << endl;
		for (int i = 0; i < nCol; i ++)
			cout << "Column " << i << " : " << endl
				<< '\t' << (ans[i] == -1 ? "No Concept" : kb->getConcept(ans[i])) << endl
				<< '\t' << (ans[i + nCol] == -1 ? "No Relation" : kb->getRelation(ans[i + nCol])) << endl;
		cout <<endl;
	}

	//cache and return
	return labelCache[tid] = ans;
}
