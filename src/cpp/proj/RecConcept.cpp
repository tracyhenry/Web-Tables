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

double Bridge::distance(int c, int label, TaxoPattern *cp)
{
	double dis = 0;
	for (int lca = c; kb->getPreCount(lca); )
		if (! cp->c.count(lca))
			dis ++, lca = kb->getPreNode(lca, 0);
		else
			break;
	if (label != -1)
		for (int lca = label; kb->getPreCount(lca); )
			if (! kb->isDescendant(c, lca))
				dis ++, lca = kb->getPreNode(lca, 0);
			else
				break;
	return dis;
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

		//LCA dis
		double dis = distance(c, labels[entityCol], cellPattern[entityCellId]);

		//loop over all attributes
		double sumSim = 0;
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

void Bridge::dfsPrune(int x, int r, int K, Table curTable)
{
	//Table information
	int nCol = curTable.nCol;
	int entityCol = curTable.entityCol;
	if (entityCol == -1)
		entityCol = 0;
	int entityCellId = curTable.cells[r][entityCol].id;

	//column concept & relationship labels
	vector<int> labels = findColConceptAndRelation(curTable.table_id, false);

	//we reach a leaf
	if (! kb->getSucCount(x))
	{
		//LCA dis
		double dis = distance(x, labels[entityCol], cellPattern[entityCellId]);

		//loop over all attributes
		double sumSim = 0;
		for (int j = 0; j < nCol; j ++)
		{
			if (j == entityCol) continue;
			if (labels[j + nCol] == -1 ||
				! conSchema[x].count(labels[j + nCol]))
					continue;
			//patternSim
			TaxoPattern *p1 = cellPattern[curTable.cells[r][j].id];
			TaxoPattern *p2 = conSchema[x][labels[j + nCol]];
			double sim = Matcher::patternSim(kb, p1, p2, Param::recConceptSim);
			sumSim += sim / exp(log(Param::DISEXPBASE) * dis);
		}

		//current pair
		auto cp = make_pair(make_pair(-sumSim, dis), x);
		if ((int) heap.size() < K)
			heap.push(cp);
		else if (cp < heap.top())
			heap.pop(), heap.push(cp);

		return;
	}

	//otherwise, x is a non-leaf node, we sort its children by upper bounds
	vector<pair<double, int>> children;
	int sucCount = kb->getSucCount(x);
	for (int i = 0; i < sucCount; i ++)
	{
		//current child
		int curChild = kb->getSucNode(x, i);

		//check leaf
		if (! kb->getSucCount(curChild))
		{
			dfsPrune(curChild, r, K, curTable);
			continue;
		}

		//minimal distance
		double minDis = 0;
		for (int lca = curChild; kb->getPreCount(lca); )
			if (! cellPattern[entityCellId]->c.count(lca))
				minDis ++, lca = kb->getPreNode(lca, 0);
			else
				break;
		if (labels[entityCol] != -1)
			if (! kb->isDescendant(labels[entityCol], curChild))
				for (int lca = labels[entityCol]; kb->getPreCount(lca); )
					if (! kb->isDescendant(curChild, lca))
						minDis ++, lca = kb->getPreNode(lca, 0);
					else
						break;

		//maximal sim
		double maxSim = 0;
		for (int j = 0; j < nCol; j ++)
		{
			if (j == entityCol) continue;
			if (labels[j + nCol] == -1 ||
				! conSchema[curChild].count(labels[j + nCol]))
					continue;

			TaxoPattern *p1 = conSchema[curChild][labels[j + nCol]];
			TaxoPattern *p2 = cellPattern[curTable.cells[r][j].id];
			TaxoPattern *p3 = new TaxoPattern();
			switch (Param::recConceptSim)
			{
				case DotProduct :
					p3->numEntity = 1.0;
					for (auto kv : p2->c)
						if (p1->c.count(kv.first))
							p3->c[kv.first] = 1.0;
					for (auto kv : p2->e)
						if (p1->e.count(kv.first))
							p3->e[kv.first] = 1.0;
					maxSim += Matcher::patternSim(kb, p2, p3, Param::recConceptSim);
					break;

				case Jaccard :
					break;

				case Dice :
					break;

				case wJaccard :
					break;
			}
		}

		//upper bound
		double ub = maxSim / exp(log(Param::DISEXPBASE) * minDis);
		children.emplace_back(- ub, curChild);
	}
	sort(children.begin(), children.end());

	//dfs
	for (auto kv : children)
	{
		double ub = - kv.first;
		int child = kv.second;

		//prune
		if ((int) heap.size() == K && ub < - heap.top().first.first)
			continue;
		dfsPrune(child, r, K, curTable);
	}
}

vector<int> Bridge::fastFindRecordConcept(int tid, int r, int K, bool print)
{
	heap = priority_queue<pair<pair<double, double>, int>>();
	Table curTable = corpus->getTableByDataId(tid);
	int kbRoot = kb->getRoot();

	//call dfsPrune
	dfsPrune(kbRoot, r, K, curTable);

	//gather answer
	vector<pair<pair<double, double>, int>> simScore;
	while (! heap.empty())
	{
		simScore.push_back(heap.top());
		heap.pop();
	}
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
