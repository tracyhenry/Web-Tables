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

	while (1)
	{
		cout << endl << "-----------------------------------------" << endl;
		cout << "Input table_id and row_id :" << endl;
		int x, y;
		cin >> x >> y;
		bridge->findConcept(x, y);
		cout << endl << "-----------------------------------------" << endl;
	}

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
