#include "Bridge.h"
#include <map>
#include <vector>
#include <string>
#include <fstream>
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
//	bridge->traverse();
//	bridge->findAllRelation();
	bridge->findAllConcept();

/*	while (1)
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
	return 0;
}
