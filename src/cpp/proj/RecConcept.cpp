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
			recConcept[i][r] = findRecordConcept(curTable.table_id, r, 30, false);
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
vector<int> Bridge::findRecordConcept(int tid, int r, int K, bool print)
{
	//Brute force
	int totalConcept = kb->countConcept();

	//Similarity Array
	vector<pair<pair<double, double>, int>> simScore;
	simScore.clear();

	//current table
	Table curTable = corpus->getTableByDataId(tid);
	int nCol = curTable.nCol;
	int entityCol = curTable.entityCol;
	if (entityCol == -1)
		entityCol = 0;
	int entityCellId = curTable.cells[r][entityCol].id;

	//column concepts and relationships
	vector<int> labels = findColConceptAndRelation(tid, false);

	//loop over all concepts
	for (int c = 1; c <= totalConcept; c ++)
	{
		if (kb->getSucCount(c)) continue;
		double sumSim = 0;

		//LCA dis
		double dis = 0;
		for (int lca = c; kb->getPreCount(lca); )
			if (! cellPattern[entityCellId]->c.count(lca))
				dis ++, lca = kb->getPreNode(lca, 0);
			else
				break;
		if (labels[entityCol] != -1)
			for (int lca = labels[entityCol]; kb->getPreCount(lca); )
				if (! kb->isDescendant(c, lca))
					dis ++, lca = kb->getPreNode(lca, 0);
				else
					break;
		//loop over all attributes
		for (int j = 0; j < nCol; j ++)
		{
			if (j == entityCol) continue;
			if (labels[j + nCol] == -1 ||
				! conSchema[c].count(labels[j + nCol]))
					continue;
			//patternSim
			TaxoPattern *p1 = cellPattern[curTable.cells[r][j].id];
			TaxoPattern *p2 = conSchema[c][labels[j + nCol]];
			double sim = Matcher::patternSim(kb, p1, p2, Param::recConceptSim);
			//combine
			sumSim += sim / exp(log(Param::DISEXPBASE) * dis);
		}
		simScore.emplace_back(make_pair(-sumSim, dis), c);
	}

	//sort
	sort(simScore.begin(), simScore.end());

	//output
	if (print)
	{
		cout << endl << "Top " << K << " Answers: " << endl;
		for (int i = 0; i < min((int) simScore.size(), K); i ++)
		{
			cout << i << ":\t" << - simScore[i].first.first << " " << simScore[i].first.second
				<< " " << kb->getConcept(simScore[i].second)
				<< " " << kb->getDepth(simScore[i].second) << endl;
			cout << endl;
		}
	}

	//return top k answers
	vector<int> ans;
	for (int i = 0; i < min((int) simScore.size(), K); i ++)
		ans.push_back(simScore[i].second);

	return ans;
}

/**
* Given a table id and a row id
* output the a ranked concept list for
* the records represented by the query row
* The baseline method.
*/
vector<int> Bridge::baselineFindRecordConcept(int tid, int r, int K, bool print)
{
	//similarity array
	vector<pair<double, int>> simScore;

	//current table
	Table curTable = corpus->getTableByDataId(tid);
	int entityCol = curTable.entityCol;
	if (entityCol == -1)
		entityCol = 0;
	int entityCellId = curTable.cells[r][entityCol].id;
	TaxoPattern *cp = cellPattern[entityCellId];
	vector<int> labels = findColConceptAndRelation(tid, false);

	//enumerate concepts in the cell pattern
	for (auto kv : cp->c)
	{
		int c = kv.first;
		if (kb->getSucCount(c)) continue;

		//LCA dis
		double dis = 0;
		for (int lca = c; kb->getPreCount(lca); )
			if (! cp->c.count(lca))
				dis ++, lca = kb->getPreNode(lca, 0);
			else
				break;
		if (labels[entityCol] != -1)
			for (int lca = labels[entityCol]; kb->getPreCount(lca); )
				if (! kb->isDescendant(c, lca))
					dis ++, lca = kb->getPreNode(lca, 0);
				else
					break;
		simScore.emplace_back(dis, c);
	}

	//sort
	sort(simScore.begin(), simScore.end());

	//output
	if (print)
	{
		cout << endl << "Top " << K << " Answers: " << endl;
		for (int i = 0; i < min((int) simScore.size(), K); i ++)
		{
			cout << i << ":\t" << simScore[i].first << " "
				<< " " << kb->getConcept(simScore[i].second)
				<< " " << kb->getDepth(simScore[i].second) << endl;
			cout << endl;
		}
	}

	//return top k answers
	vector<int> ans;
	for (int i = 0; i < min((int) simScore.size(), K); i ++)
		ans.push_back(simScore[i].second);

	return ans;
}
