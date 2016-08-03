#include "Bridge.h"
#include "Matcher.h"
#include <vector>
#include <iomanip>
#include <algorithm>
#include <iostream>
using namespace std;

/**
* call findRelation for all columns
*/
void Bridge::findAllRelation()
{
	int nTable = corpus->countTable();
	colRelation.resize(nTable + 1);

	for (int i = 1; i <= nTable; i ++)
	{
		if (i % 1000 == 0)
			cout << i << endl;
		Table curTable = corpus->getTable(i);
		colRelation[i].resize(curTable.nCol);

		for (int c = 0; c < curTable.nCol; c ++)
			colRelation[i][c] = findRelation(curTable.table_id, c, false);
	}

	//output to a result file
	ofstream fout("../../../data/Result/colRelation/colRelation.txt");
	for (int i = 1; i <= nTable; i ++)
		for (int c = 0; c < corpus->getTable(i).nCol; c ++)
			if (colRelation[i][c].size())
				fout << corpus->getTable(i).table_id << " "
					<< c << " " << kb->getRelation(colRelation[i][c][0])
					<< endl;
	fout.close();
}

/**
* Given a table id and a column id,
* return the a ranked list of relationships between
* the query column and the entity column
*/
int Bridge::naiveFindRelation(int tid, int c, bool print)
{
	//Table variables
	Table curTable = corpus->getTableByDataId(tid);
	int entityCol = curTable.entityCol;
	int nRow = curTable.nRow;

	if (c < 0 || c >= curTable.nCol || entityCol == c || entityCol == -1)
	{
		if (print)
		{
			if (c < 0 || c >= curTable.nCol)
				cout << "Sorry, index out of range!" << endl;
			else if (entityCol == -1)
				cout << "Sorry, this table dosen't have a given entity column." << endl;
			else
				cout << "Sorry, the column you are querying for is the same as the entity column!"
					<< endl;
		}
		return -1;
	}

	//KB constants
	int R = kb->countRelation();

	//majority
	int ans = -1, maxHit = -1;
	for (int rel = 1; rel <= R; rel ++)
	{
		int totalHit = 0;
		for (int i = 0; i < nRow; i ++)
		{
			int c1 = curTable.cells[i][entityCol].id;
			int c2 = curTable.cells[i][c].id;
			unordered_set<int> ent1, ent2;
			for (auto kv : matches[c1])
				ent1.insert(kv.first);
			for (auto kv : matches[c2])
				ent2.insert(kv.first);
			bool hit = false;
			for (int e1 : ent1)
			{
				int totalFactCount = kb->getFactCount(e1);
				for (int k = 0; k < totalFactCount; k ++)
				{
					pair<int, int> cp = kb->getFactPair(e1, k);
					if (cp.first == rel && ent2.count(cp.second))
					{
						hit = true;
						break;
					}
				}
				if (hit)
					break;
			}
			if (hit)
				totalHit ++;
		}
		if (totalHit > maxHit)
			maxHit = totalHit, ans = rel;
	}
	return ans;
}

/**
* Given a table id and a column id,
* return the a ranked list of relationships between
* the query column and the entity column
*/
vector<int> Bridge::findRelation(int tid, int c, bool print)
{
	//answer
	vector<int> ans;

	//Table variables
	Table curTable = corpus->getTableByDataId(tid);
	int entityCol = curTable.entityCol;

	if (c < 0 || c >= curTable.nCol || entityCol == c || entityCol == -1)
	{
		if (print)
		{
			if (c < 0 || c >= curTable.nCol)
				cout << "Sorry, index out of range!" << endl;
			else if (entityCol == -1)
				cout << "Sorry, this table dosen't have a given entity column." << endl;
			else
				cout << "Sorry, the column you are querying for is the same as the entity column!"
					<< endl;
		}
		return ans;
	}

	//KB constants
	int H = kb->getDepth(kb->getRoot());
	int R = kb->countRelation();

	//Similarity Array
	vector<pair<depthVector, int>> simScore;
	simScore.clear();

	for (int r = 1; r <= R; r ++)
	{
		depthVector sumSim(H + 1);

		//loop over all row/record
		for (int row = 0; row < curTable.nRow; row ++)
		{
			int cellId = curTable.cells[row][entityCol].id;
			for (int i = 0; i < (int) matches[cellId].size(); i ++)
			{
				int e = matches[cellId][i].first;
				if (! entSchema[e].count(r))
					continue;

				TaxoPattern *cp = cellPattern[curTable.cells[row][c].id];
				TaxoPattern *pp = entSchema[e][r];
				depthVector curVector = Matcher::dVector(kb, cp, pp);

				sumSim.addUpdate(curVector);
			}
		}
		simScore.emplace_back(sumSim, r);
	}

	sort(simScore.begin(), simScore.end());
	//output
	if (print)
	{
		cout << endl << "Top 10 Answers: " << endl;
		for (int i = 0; i < min((int) simScore.size(), 10); i ++)
		{
			cout << simScore[i].first.score(1000.0) << " " << simScore[i].second
				<< " " << kb->getRelation(simScore[i].second) << endl;

			for (int j = H; j > 16; j --)
				cout << left << setw(15) << simScore[i].first.w[j] << " ";
			cout << endl << endl;
		}
	}

	//currently a threshold-based approach
	for (int i = 0; i < (int) simScore.size(); i ++)
		if (simScore[i].first.score(1000.0) > 1e57)
			ans.push_back(simScore[i].second);

	return ans;
}
