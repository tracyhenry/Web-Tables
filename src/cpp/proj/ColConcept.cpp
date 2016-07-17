#include "Bridge.h"
#include "Matcher.h"
#include <cstring>
#include <algorithm>
#include <unordered_set>
#include <iostream>
using namespace std;

double Bridge::getNumLuckyCells(Table curTable, int c)
{
	double numLuckyCell = 0;
	for (int i = 0; i < curTable.nRow; i ++)
		if (matches[curTable.cells[i][c].id].size())
		{
			if (matches[curTable.cells[i][c].id][0].second == 1.0)
				numLuckyCell += 1.0;
			else
				numLuckyCell += WT_SEMILUCKY;
		}
	return numLuckyCell;
}

double Bridge::getNumContainedCells(Table curTable, int c, int conceptId)
{
	double numContainedCell = 0;
	for (int j = 0; j < curTable.nRow; j ++)
	{
		vector<pair<int, double>>& curMatches = matches[curTable.cells[j][c].id];
		if (curMatches.empty())
			continue;
		for (auto kv: curMatches)
		{
			int entityId = kv.first;
			if (kb->checkRecursiveBelong(entityId, conceptId))
			{
				numContainedCell += (curMatches[0].second == 1.0 ? 1.0 : WT_SEMILUCKY);
				break;
			}
		}
	}
	return numContainedCell;
}

/**
 * Given a table_id and column number,
 * output top-k concepts describing this column.
 * This is the Naive Majority method.
 */

vector<int> Bridge::findColConceptMajority(int tid, int c, bool print)
{
	Table curTable = corpus->getTableByDataId(tid);
	//check range
	if (c < 0 || c >= curTable.nCol)
	{
		cout << "Column index is out of range!" << "   " << tid << " " << c << endl;
		return vector<int>();
	}

	//Compute total lucky cell in this column
	double numLuckyCell = getNumLuckyCells(curTable, c);

	//Make candidate set from column pattern
	unordered_set<int> candidates;
	for (auto kv : colPattern[curTable.id][c]->c)
		candidates.insert(kv.first);

	//Loop over all concepts
	vector<pair<int, int>> score;
	for (int conceptId : candidates)
	{
		double numContainedCell = getNumContainedCells(curTable, c, conceptId);
		double luckyRate = numLuckyCell / curTable.nRow;
		double threshold = TMIN + (TMAX - TMIN) * luckyRate;
		if (numContainedCell / numLuckyCell >= threshold)
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

/**
* Given a table id,
* output a vector of int with the first nCol elements being
* the concept IDs and the last nCol elements being the
* relation IDs.
* For the entity column, the rel will be -1.
*/

vector<int> Bridge::findColConceptAndRelation(int tid, bool print)
{
	Table curTable = corpus->getTableByDataId(tid);
	int nRow = curTable.nRow;
	int nCol = curTable.nCol;
	int numRelation = kb->countRelation();
	int entityCol = curTable.entityCol;
	int H = kb->getDepth(kb->getRoot());
	vector<int> ans(nCol * 2, -1);
	depthVector ansDv(H + 1);

	//check if there is a given entity column
	if (entityCol == -1)
	{
		cout << "There is no entity column given! Quit..." << endl;
		return ans;
	}

	//make a candidate concept set for each column
	cout << "Table_id = " << tid << endl;
	vector<vector<int>> candidates(nCol);
	for (int i = 0; i < nCol; i ++)
	{
		double numLuckyCell = getNumLuckyCells(curTable, i);
		double luckyRate = (double) numLuckyCell / nRow;
		double threshold = TMIN + (TMAX - TMIN) * luckyRate;
//		cout << "Threshold for column " << i <<" : " << threshold << endl;
		for (auto kv : colPattern[curTable.id][i]->c)
		{
			int conceptId = kv.first;
			double numContainedCell = getNumContainedCells(curTable, i, conceptId);
			if (numContainedCell / numLuckyCell >= threshold)
				candidates[i].push_back(conceptId);
		}
/*		cout << "Candidate size for column " << i << " : " << candidates[i].size() << endl;
		if (candidates[i].size() <= 15)
		{
			cout << "Candidates for this column : " << endl;
			for (int conceptId : candidates[i])
				cout << '\t' << kb->getConcept(conceptId) << endl;
		}
*/
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

	//preprocess something
	vector<vector<int>> attrConcepts(nCol, vector<int>(numRelation + 1, -1));
	vector<vector<depthVector>> bestDvs(nCol, vector<depthVector>(numRelation + 1));
	for (int i = 0; i < nCol; i ++)
	{
		if (i == entityCol)
			continue;
		for (int rel = 1; rel <= numRelation; rel ++)
		{
			attrConcepts[i][rel] = -1;
			bestDvs[i][rel].w.resize(H + 1);
			for (int conceptId : candidates[i])
			{
				if (! conSchema[conceptId].count(rel))
					continue;
				TaxoPattern *p1 = colPattern[curTable.id][entityCol];
				TaxoPattern *p2 = conSchema[conceptId][rel];
				depthVector cur = Matcher::dVector(kb, p1, p2);
				if (cur < bestDvs[i][rel])
				{
					bestDvs[i][rel] = cur;
					attrConcepts[i][rel] = conceptId;
				}
			}
		}
	}

	for (int entityColConcept : candidates[entityCol])
	{
		depthVector sumDv(H + 1);
		vector<int> curState(nCol * 2, -1);
		curState[entityCol] = entityColConcept;
		for (int i = 0; i < nCol; i ++)
		{
			if (i == entityCol) continue;
			depthVector bestDv(H + 1);
			for (int rel = 1; rel <= numRelation; rel ++)
			{
				if (! conSchema[entityColConcept].count(rel))
					continue;
				int reverseRel = kb->getReverseRelationId(rel);
				TaxoPattern *p1 = colPattern[curTable.id][i];
				TaxoPattern *p2 = conSchema[entityColConcept][rel];
				depthVector curDv = Matcher::dVector(kb, p1, p2);
				curDv.addUpdate(bestDvs[i][reverseRel]);
				if (curDv < bestDv)
				{
					bestDv = curDv;
					curState[i] = attrConcepts[i][reverseRel];
					curState[i + nCol] = rel;
				}
			}
			sumDv.addUpdate(bestDv);
		}
		if (sumDv < ansDv)
		{
			ansDv = sumDv;
			ans = curState;
		}
	}

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
	return ans;
}
