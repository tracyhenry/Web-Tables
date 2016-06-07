#include "Bridge.h"
#include <map>
#include <ctime>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sys/time.h>
using namespace std;

int main()
{
/*
	cout << wwt->countMultiColumnTable() << endl;

	//test Corpus public functions
	int tid = (wwt->getCellById(25)).table_id;
	cout << "Current Table: " << tid << endl;
	Table curTable = wwt->getTableById(tid);
	cout << curTable.nRow << " " << curTable.nCol << endl;

	for (int i = 0; i < curTable.nRow; i ++)
		for (int j = 0; j < curTable.nCol; j ++)
			cout << curTable.cells[i][j].id << endl;
*/

	Bridge *bridge = new Bridge();
//	bridge->traverse();
//	bridge->findAllRelation();
//	bridge->findAllConcept();
/*
	while (1)
	{
		cout << endl << "-----------------------------------------" << endl;
		cout << "Input table_id and row_id :" << endl;
		int t, x, y;
		cin >> t >> x >> y;
		switch (t)
		{
			case 0:
				bridge->findConcept(x, y, true);
				break;
			case 1:
				bridge->findRelation(x, y, true);
				break;
		}
		cout << endl << "-----------------------------------------" << endl;
	}
*/
//	bridge->tableQuery();
//	bridge->testPattern();

	//Test findRelation running time
	int nTable = bridge->corpus->countTable();
	double totalTime = 0;

	srand(time(0));
	for (int lp = 1; lp <= 1000; lp ++)
	{
		int curTable = rand() % nTable + 1;
		int col = -1;
		for (int t = 0; t < 50; t ++)
		{
			int nCol = bridge->corpus->getTable(curTable).nCol;
			int entityCol = bridge->corpus->getTable(curTable).entityCol;
			if (col >= 0 && col < nCol && col != entityCol)
				break;
			col = rand() % nCol;
		}

		struct timeval t1, t2;
		gettimeofday(&t1, NULL);
		bridge->findRelation(bridge->corpus->getTable(curTable).table_id, col, true);
		gettimeofday(&t2, NULL);

		totalTime += t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) / 1000000.0;
		if (lp % 50 == 0)
			cout << "haha : " << lp / 50 << endl;
	}
	cout << "Average findRelation latency: " << totalTime / 1000.0 << endl;
	return 0;
}
