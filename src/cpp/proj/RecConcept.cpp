#include "Bridge.h"
#include "Matcher.h"
#include <vector>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <algorithm>
#define IterID unordered_map<int, double>::iterator
#define IterIT unordered_map<int, TaxoPattern *>::iterator
using namespace std;

/*
call findConcept for all records
**/
void Bridge::findAllConcept()
{
	int nTable = corpus->countTable();
	recConcept.resize(nTable + 1);

	for (int i = 1; i <= nTable; i ++)
	{
		Table curTable = corpus->getTable(i);
		if (curTable.entityCol == -1)
			continue;

		recConcept[i].resize(curTable.nRow);
		for (int r = 0; r < curTable.nRow; r ++)
			recConcept[i][r] = findRecordConcept(curTable.table_id, r, false);
	}
	//output to a file
	ofstream fout("../../../data/Result/recConcept/recConcept.txt");
	for (int i = 1; i <= nTable; i ++)
		for (int r = 0; r < (int) recConcept[i].size(); r ++)
			for (int k = 0; k < (int) recConcept[i][r].size(); k ++)
				fout << corpus->getTable(i).table_id << " " << r << " " << k
					<< " " << kb->getConcept(recConcept[i][r][k]) << endl;
	fout.close();
}

/**
* Given a table id and a row id
* output the a ranked concept list for
* the record represented by the query row
*/
vector<int> Bridge::findRecordConcept(int tid, int r, bool print)
{
	//Brute force
	int totalConcept = kb->countConcept();

	//Similarity Array
	vector<pair<double, int>> simScore;
	simScore.clear();

	//information about the current record
	Table curTable = corpus->getTableByDataId(tid);
	int nCol = curTable.nCol;
	int entityCol = curTable.entityCol;
	if (entityCol == -1)
		entityCol = 0;

	//loop over all concepts
	for (int i = 1; i <= totalConcept; i ++)
	{
		if (kb->getSucCount(i)) continue;
		double sumSim = 0;

		//loop over all attributes
		for (int c = 0; c < nCol; c ++)
		{
			if (c == entityCol) continue;
			double sim = 0;

			//loop over all properties
			for (IterIT it1 = conSchema[i].begin();
				it1 != conSchema[i].end(); it1 ++)
			{
				if (it1->first!= kb->getRelationId("isLocatedIn"))
					continue;
				TaxoPattern *cp = cellPattern[curTable.cells[r][c].id];
				TaxoPattern *pp = it1->second;

				//overall matching
				double curSim = Matcher::patternSim(kb, cp, pp);
				sim = max(sim, curSim);
			}
			sumSim += sim;
		}
		simScore.emplace_back(-sumSim, i);
	}
	//sort
	sort(simScore.begin(), simScore.end());

	//output
	if (print)
	{
		cout << endl << "Top 30 Answers: " << endl;
		for (int i = 0; i < min((int) simScore.size(), 30); i ++)
		{
			cout << - simScore[i].first << " " << simScore[i].second
				<< " " << kb->getConcept(simScore[i].second)
				<< " " << kb->getDepth(simScore[i].second) << endl;
			cout << endl;
		}
	}
	//return top 3 answers
	vector<int> ans;
	for (int i = 0; i < min((int) simScore.size(), 5); i ++)
		ans.push_back(simScore[i].second);

	return ans;
}
