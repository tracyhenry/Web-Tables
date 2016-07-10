#include "Bridge.h"
#include <map>
#include <ctime>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <sys/time.h>
using namespace std;

Bridge *bridge;

void genRandomRecords()
{
	int num_records = 200;
	int records_per_table = 5;
	int nTable = bridge->corpus->countTable();
	double lucky_rate_threshold = 0.4;
	string file_name = "../../../data/GT/random_records.txt";
	unordered_set<int> chosen_tables;
	srand(time(0));

	//open file
	ofstream fout(file_name.c_str());

	for (int it1 = 1; it1 <= num_records / records_per_table; it1 ++)
	{
		//randomly choose a table
		int tid;
		Table curTable;
		while (1)
		{
			tid = rand() % nTable + 1;
			//if chosen, continue
			if (chosen_tables.count(tid))
				continue;

			curTable = bridge->corpus->getTable(tid);
			//if less than records_per_table lines, continue
			if (curTable.nRow < records_per_table)
				continue;

			//check if there is at least one lucky attribute column
			bool has_lucky_attr_col = false;
			bool has_lucky_entity_col = false;
			for (int i = 0; i < curTable.nCol; i ++)
			{
				int num_lucky_cell = 0;
				for (int j = 0; j < curTable.nRow; j ++)
					if (! bridge->getMatch(curTable.cells[j][i].id).empty())
						num_lucky_cell ++;
				double lucky_rate = num_lucky_cell / (double) curTable.nRow;
				if (lucky_rate >= lucky_rate_threshold)
					if (i == curTable.entityCol)
						has_lucky_entity_col = true;
					else
						has_lucky_attr_col = true;
			}

			//if there is no lucky attr col nor entity col, continue
			if (! has_lucky_attr_col or ! has_lucky_entity_col)
				continue;

			//ok, this table is chosen now
			chosen_tables.insert(tid);
			break;
		}

		//randomly choose records_per_table records
		unordered_set<int> chosen_records;
		for (int it2 = 1; it2 <= records_per_table; it2 ++)
		{
			int cur_record;
			while (1)
			{
				cur_record = rand() % curTable.nRow;
				if (! chosen_records.count(cur_record))
					break;
			}
			chosen_records.insert(cur_record);
			fout << curTable.table_id << " " << cur_record << endl;
		}
		fout << endl;
	}
	fout.close();
}

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

	bridge = new Bridge();
//	genRandomRecords();
//	bridge->traverse();
//	bridge->findAllRelation();
//	bridge->findAllConcept();

	while (1)
	{
		cout << endl << "-----------------------------------------" << endl;
		cout << "Input table_id and row_id :" << endl;
		int t, x, y;
		cin >> t >> x >> y;
		switch (t)
		{
			case 0:
				bridge->findRecordConcept(x, y, true);
				break;
			case 1:
				bridge->findRelation(x, y, true);
				break;
		}
		cout << endl << "-----------------------------------------" << endl;
	}

//	bridge->tableQuery();
//	bridge->testPattern();

/*	//Test findRelation running time
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


	//Test findConcept running time
	int nTable = bridge->corpus->countTable();
	double totalTime = 0;

	srand(time(0));
	for (int lp = 1; lp <= 1000; lp ++)
	{
		int curTable = rand() % nTable + 1;
		int row = rand() % bridge->corpus->getTable(curTable).nRow;
		cout << curTable << " " << row << endl;
		struct timeval t1, t2;
		gettimeofday(&t1, NULL);
		bridge->findConcept(bridge->corpus->getTable(curTable).table_id, row, true);
		gettimeofday(&t2, NULL);

		totalTime += t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) / 1000000.0;
	}
	cout << "Average findConcept latency: " << totalTime / 1000.0 << endl;

	//Output the maximum running time
	vector<int> nCols;
	int nTable = bridge->corpus->countTable();
	for (int i = 0; i < nTable; i ++)
		nCols.push_back(bridge->corpus->getTable(i + 1).nCol);
	sort(nCols.begin(), nCols.end());
	for (int x : nCols)
		cout << x << endl;
*/
	return 0;
}
