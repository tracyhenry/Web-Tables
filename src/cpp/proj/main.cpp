#include "Bridge.h"
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
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
	bridge->traverse();
	int nTable = bridge->corpus->countTable();
	int goodTable = 0;
	for (int i = 1; i <= nTable; i ++)
	{
		if (i % 500 == 0)
			cout << i << endl;
		Table curTable = bridge->corpus->getTable(i);
		int goodCol = 0;

		if (curTable.entityCol < 0)
			continue;

		for (int c = 0; c < curTable.nCol; c ++)
		{
			if (c == curTable.entityCol)
				continue;
			goodCol += bridge->findRelation(curTable.table_id, c);
		}
		if (goodCol)
			goodTable ++;
	}
	cout << "Number of good tables: " << goodTable << endl;

/*	while (1)
	{
		cout << endl << "-----------------------------------------" << endl;
		cout << "Input table_id and row_id :" << endl;
		int t, x, y;
		cin >> t >> x >> y;
		switch (t)
		{
			case 0:
				bridge->findConcept(x, y);
				break;
			case 1:
				bridge->findRelation(x, y);
				break;
		}
		cout << endl << "-----------------------------------------" << endl;
	}
*/

/*	bridge->tableQuery();
	bridge->testPattern();
	int totalConcept = bridge->kb->countConcept();
	int notUnderThing = -1;
	for (int i = 1; i <= totalConcept; i ++)
		if (bridge->kb->getPreCount(i) != 1)
			notUnderThing ++;
	cout << notUnderThing << endl; */
	return 0;
}
