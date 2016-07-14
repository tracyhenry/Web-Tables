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
	vector<pair<depthVector, int>> simScore;
	simScore.clear();

	//information about the current record
	Table curTable = corpus->getTableByDataId(tid);
	int nCol = curTable.nCol;
	int entityCol = curTable.entityCol;
	if (entityCol == -1)
		entityCol = 0;
	int cid = curTable.cells[r][entityCol].id;

	//depth of kb
	int H = kb->getDepth(kb->getRoot());

	//debug
//	getconSchema(70366, kb->getRelationId("created"), true);
//	getconSchema(114102, kb->getRelationId("created"), true);
//	getCellPattern(curTable.cells[r][entityCol].id, true);
//	for (int i = 1; i <= kb->countRelation(); i ++)
//		getconSchema(25431, i, true);

	//loop over all concepts
	for (int i = 1; i <= totalConcept; i ++)
	{
		if (! cellPattern[cid]->c.count(i))
			continue;
		if (kb->getSucCount(i)) continue;

		depthVector sumSim(H + 1);

		//extras
		vector<int> extraEntity;
		for (int j = 0; j < (int) matches[cid].size(); j ++)
		{
			int curEntity = matches[cid][j].first;
			if (kb->checkBelong(curEntity, i))
				extraEntity.push_back(curEntity);
		}
		//loop over all attributes
		for (int c = 0; c < nCol; c ++)
		{
			if (c == entityCol) continue;
			depthVector sim(H + 1);

			//loop over all properties
			for (IterIT it1 = conSchema[i].begin();
				it1 != conSchema[i].end(); it1 ++)
			{
				TaxoPattern *cp = cellPattern[curTable.cells[r][c].id];
				TaxoPattern *pp = it1->second;

				//overall matching
				depthVector curVector = Matcher::dVector(kb, cp, pp);
				//subtract extra
				for (int j = 0; j < (int) extraEntity.size(); j ++)
				{
					int curEntity = extraEntity[j];
					if (entSchema[curEntity].count(it1->first))
					{
						pp = entSchema[curEntity][it1->first];
						depthVector sub = Matcher::dVector(kb, cp, pp);
						sub.normalize(-1);
						curVector.addUpdate(sub);
					}
				}
				sim.maxUpdate(curVector);
			}
			sumSim.addUpdate(sim);
		}
		//normalize
		double f = kb->getPossessCount(i) - (int) extraEntity.size();
		if (f > 0)
			sumSim.normalize(kb->getPossessCount(i));

		simScore.emplace_back(sumSim, i);
	}
	//sort
	sort(simScore.begin(), simScore.end());

	//output
	if (print)
	{
		cout << endl << "Top 30 Answers: " << endl;
		for (int i = 0; i < min((int) simScore.size(), 30); i ++)
		{
			cout << simScore[i].first.score(1000.0) << " " << simScore[i].second
				<< " " << kb->getConcept(simScore[i].second)
				<< " " << kb->getDepth(simScore[i].second) << endl;

			for (int j = H; j > 16; j --)
				cout << left << setw(15) << simScore[i].first.w[j] << " ";
			cout << endl << endl;
		}
	}
	//return top 3 answers
	vector<int> ans;
	for (int i = 0; i < min((int) simScore.size(), 5); i ++)
		ans.push_back(simScore[i].second);

	return ans;
}
