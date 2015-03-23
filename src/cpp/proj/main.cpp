#include "KB.h"
#include "Corpus.h"
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
using namespace std;

//KB *yago = new YAGO();
Corpus *wwt = new WWT();

int main()
{
//	yago->Traverse();
//	cout << wwt->countMultiColumnTable() << endl;

	//test Corpus public functions
	int tid = (wwt->getCellById(25)).table_id;
	cout << "Current Table: " << tid << endl;
	Table curTable = wwt->getTableById(tid);
	cout << curTable.nRow << " " << curTable.nCol << endl;

	for (int i = 0; i < curTable.nRow; i ++)
		for (int j = 0; j < curTable.nCol; j ++)
			cout << curTable.cells[i][j].id << endl;
	return 0;
}
