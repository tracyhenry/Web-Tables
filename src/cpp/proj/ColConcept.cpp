#include "Bridge.h"
#include "Matcher.h"
#include <vector>
#include <cstring>
#include <algorithm>
#include <unordered_set>
#include <iostream>
using namespace std;

int Bridge::getNumLuckyCells(Table curTable, int c)
{
	int numLuckyCell = 0;
	for (int i = 0; i < curTable.nRow; i ++)
		if (matches[curTable.cells[i][c].id].size())
			numLuckyCell ++;
	return numLuckyCell;
}

int Bridge::getNumContainedCells(Table curTable, int c, int conceptId)
{
	int numContainedCell = 0;
	for (int j = 0; j < curTable.nRow; j ++)
	{
		vector<int>& curMatches = matches[curTable.cells[j][c].id];
		if (curMatches.empty())
			continue;
		for (int entityId : curMatches)
			if (kb->checkRecursiveBelong(entityId, conceptId))
			{
				numContainedCell ++;
				break;
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
	double majorityThreshold = 0.8;

	//check range
	if (c < 0 || c >= curTable.nCol)
	{
		cout << "Column index is out of range!" << "   " << tid << " " << c << endl;
		return vector<int>();
	}

	//Compute total lucky cell in this column
	int numLuckyCell = getNumLuckyCells(curTable, c);

	//Make candidate set from column pattern
	unordered_set<int> candidates;
	for (auto kv : colPattern[curTable.id][c]->c)
		candidates.insert(kv.first);

	//Loop over all concepts
	vector<pair<int, int>> score;
	for (int conceptId : candidates)
	{
		int numContainedCell = getNumContainedCells(curTable, c, conceptId);
		if ((double) numContainedCell / numLuckyCell >= majorityThreshold)
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
	int nCol = curTable.nCol;
	int numRelation = kb->countRelation();
	int entityCol = curTable.entityCol;
	int H = kb->getDepth(kb->getRoot());
	int firstElement = -1;
	double majorityThreshold = 0.7;
	vector<int> ans(nCol * 2, -1), curState(nCol * 2, -1);
	depthVector dvAns(H + 1), dvCurState(H + 1);

	//check if there is a given entity column
	if (entityCol == -1)
	{
		cout << "There is no entity column given! Quit..." << endl;
		return ans;
	}

	//make a candidate concept set for each column
	vector<vector<int>> candidates(nCol);
	for (int i = 0; i < nCol; i ++)
	{
		int numLuckyCell = getNumLuckyCells(curTable, i);
		for (auto kv : colPattern[curTable.id][i]->c)
		{
			int conceptId = kv.first;
			int numContainedCell = getNumContainedCells(curTable, i, conceptId);
			if ((double) numContainedCell / numLuckyCell >= majorityThreshold)
				candidates[i].push_back(conceptId);
		}
	}

	//calculate search space
	int searchSpace = 1;
	for (int i = 0; i < nCol; i ++)
		searchSpace *= (candidates[i].size() ? numRelation : 1);
	for (int i = 0; i < nCol; i ++)
		searchSpace *= max((int) candidates[i].size(), 1);
	if (print)
		cout << "Total search space : " << searchSpace << endl;

	//brute-force
	for (int i = 0; i < nCol; i ++)
		if (candidates[i].size())
		{
			curState[i] = 0;
			if (firstElement == -1)
				firstElement = i;
		}
	if (firstElement == -1)
		return ans;
	while (1)
	{
		//calculate score
		dvCurState.w.clear();
		dvCurState.w.resize(H + 1);
		for (int i = 0; i < nCol; i ++)
		{
			if (i == entityCol || curState[i] == -1 || curState[entityCol] == -1)
				continue;
			int curConceptId = candidates[i][curState[i]];
			int curEntityColConceptId = candidates[entityCol][curState[entityCol]];
			depthVector maxSim(H + 1);
			//enumerate relationships
            for (int rel = 1; rel <= numRelation; rel ++)
			{
				depthVector curSim(H + 1);
				int reverseRel = kb->getReverseRelationId(rel);
				if (conSchema[curEntityColConceptId].count(rel))
					curSim.addUpdate(Matcher::dVectorJaccard(
											kb,
											colPattern[curTable.id][i],
											conSchema[curEntityColConceptId][rel]));
				if (conSchema[curConceptId].count(reverseRel))
					curSim.addUpdate(Matcher::dVectorJaccard(
											kb,
											colPattern[curTable.id][entityCol],
											conSchema[curConceptId][reverseRel]));
				if (curSim < maxSim)
					maxSim = curSim, curState[i + nCol] = rel;
			}
			dvCurState.addUpdate(maxSim);
		}
		//update ans
		if (ans[firstElement] == -1 || dvCurState < dvAns)
			ans = curState, dvAns = dvCurState;
		//go to next state
		int k = nCol - 1;
		while (k >= firstElement)
		{
			if (curState[k] == -1)
			{
				k --; continue;
			}
			if (curState[k] == (int) candidates[k].size() - 1)
				k --;
			else
				break;
		}
		if (k < firstElement)
			break;
		curState[k] ++;
		for (int i = k + 1; i < nCol; i ++)
			if (curState[i] != -1)
				curState[i] = 0;
	}
	//slightly modify ans and return it
	for (int i = 0; i < nCol; i ++)
		if (ans[i] != -1)
			ans[i] = candidates[i][ans[i]];

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
