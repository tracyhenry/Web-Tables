#include "KB.h"
#include "Corpus.h"
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
using namespace std;

KB *yago = new YAGO();
Corpus *wwt = new WWT();

int main()
{
/*	yago->traverse();
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

	int leaf = 0;
	int nConcept = yago->countConcept();
	for (int i = 1; i <= nConcept; i ++)
	{
		int nCh = yago->getAdjCount(i);
		if (nCh == 0)
			leaf ++;
	}

	cout << "Total Concept : " << nConcept << endl;
	cout << "Leaf Concepts: " << leaf << endl;

	return 0;
}
