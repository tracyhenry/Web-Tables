#include "Bridge.h"
#include "Matcher.h"
#include <vector>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <algorithm>
#define IterII unordered_map<int, int>::iterator
#define IterIT unordered_map<int, TaxoPattern *>::iterator
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

/*	//count good tables
	int goodTables = 0;
	for (int i = 1; i <= nTable; i ++)
		for (int c = 0; c < curTable.nCol; c ++)
			if (colRelation[i][c].size())
			{
				goodTables ++;
				break;
			}
	cout << "Number of good tables: " << goodTables << endl;
*/
	//output to a result file
	ofstream fout("../../../data/Result/colRelation.txt");
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
vector<int> Bridge::findRelation(int tid, int c, bool print)
{
	//answer
	vector<int> ans;

	//Table variables
	Table curTable = corpus->getTableByDataId(tid);
	int entityCol = curTable.entityCol;
	int id = curTable.id;

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
			for (int i = 0; i < matches[cellId].size(); i ++)
			{
				int e = matches[cellId][i];
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
	for (int i = 0; i < simScore.size(); i ++)
		if (simScore[i].first.score(1000.0) > 1e57)
			ans.push_back(simScore[i].second);

	return ans;
}


/**
* Given a table id and a row id
* output the a ranked concept list for 
* the record represented by the query row
*/
void Bridge::findConcept(int tid, int r)
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
		for (int j = 0; j < matches[cid].size(); j ++)
		{
			int curEntity = matches[cid][j];
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
				for (int j = 0; j < extraEntity.size(); j ++)
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
	cout << endl << "Top 50 Answers: " << endl;
	for (int i = 0; i < min((int) simScore.size(), 50); i ++)
	{
		cout << simScore[i].first.score(1000.0) << " " << simScore[i].second
			<< " " << kb->getConcept(simScore[i].second)
			<< " " << kb->getDepth(simScore[i].second) << endl;

		for (int j = H; j > 16; j --)
			cout << left << setw(15) << simScore[i].first.w[j] << " ";
		cout << endl << endl;
	}
}
