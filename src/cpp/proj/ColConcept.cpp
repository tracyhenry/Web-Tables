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
	if (! numLuckyCell)
		return vector<int>();

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

vector<int> Bridge::baselineFindColConceptAndRelation(int tid, bool print)
{
	Table curTable = corpus->getTableByDataId(tid);
	int nCol = curTable.nCol;
	vector<int> ans(nCol * 2, - 1);
	vector<int> ansColConcept;
	for (int i = 0; i < nCol; i ++)
	{
		vector<int> curAns = findColConceptMajority(tid, i, false);
		ans[i] = (curAns.size() ? curAns[0] : -1);
		curAns = findRelation(tid, i, false);
		ans[i + nCol] = (curAns.size() ? curAns[0] : -1);
	}
        if (print)
        {
                cout << endl << "Entity Column : " << curTable.entityCol << endl << endl;
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
//	cout << "Table_id = " << tid << endl;
	vector<vector<int>> candidates(nCol);
	for (int i = 0; i < nCol; i ++)
	{
		double numLuckyCell = getNumLuckyCells(curTable, i);
		double luckyRate = (double) numLuckyCell / nRow;
		double threshold = TMIN + (TMAX - TMIN) * luckyRate;
		if (! numLuckyCell) continue;
		for (auto kv : colPattern[curTable.id][i]->c)
		{
			int conceptId = kv.first;
			if (kb->getDepth(conceptId) > TH_DEPTH)
				continue;
			double numContainedCell = getNumContainedCells(curTable, i, conceptId);
			if (numContainedCell / numLuckyCell >= threshold)
				candidates[i].push_back(conceptId);
		}
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

	//preprocess column utility
	vector<double> columnUtility(nCol);
	for (int i = 0; i < nCol; i ++)
	{
		if (i == entityCol)
			continue;
		columnUtility[i] = getNumLuckyCells(curTable, i);
		columnUtility[i] /= (double) nRow;
		columnUtility[i] /= (double) (candidates[i].size() + 1);
	}

	//preprocess concept utility
	vector<unordered_map<int, double>> conceptUtility(nCol);
	for (int i = 0; i < nCol; i ++)
		for (int conceptId : candidates[i])
		{
			conceptUtility[i][conceptId] = colPattern[curTable.id][i]->c[conceptId];
			conceptUtility[i][conceptId] /= colPattern[curTable.id][i]->numEntity;
			conceptUtility[i][conceptId] /= ((kb->getDepth(conceptId) + 1) / H);
		}

	//share computation
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
				int curConcept = attrConcepts[i][reverseRel];
				TaxoPattern *p1 = colPattern[curTable.id][i];
				TaxoPattern *p2 = conSchema[entityColConcept][rel];
				depthVector curDv = Matcher::dVector(kb, p1, p2);
				curDv.addUpdate(bestDvs[i][reverseRel]);
				curDv.normalize(1.0 / columnUtility[i]);
				curDv.normalize(1.0 / conceptUtility[i][curConcept]);
				if (curDv < bestDv)
				{
					bestDv = curDv;
					curState[i] = curConcept;
					curState[i + nCol] = rel;
				}
			}
			sumDv.addUpdate(bestDv);
		}
		sumDv.normalize(1.0 / conceptUtility[entityCol][entityColConcept]);
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
