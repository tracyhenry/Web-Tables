#include "Experiment.h"
#include "Matcher.h"
#include <map>
#include <ctime>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <unordered_set>
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
				{
					if (i == curTable.entityCol)
						has_lucky_entity_col = true;
					else
						has_lucky_attr_col = true;
				}
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

void interactiveQuery()
{
	while (1)
	{
		cout << endl << "-----------------------------------------" << endl;
		cout << "Input table_id and row_id :" << endl;
		int t, x, y;
		cin >> t;
		switch (t)
		{
			case 0:
				cin >> x >> y;
				bridge->findRecordConcept(x, y, true);
				break;
			case 1:
				cin >> x >> y;
				bridge->findRelation(x, y, true);
				break;
			case 2:
				cin >> x >> y;
				bridge->findColConceptMajority(x, y, true);
				break;
			case 4:
				cin >> x;
				bridge->findColConceptAndRelation(x, true);
		}
		cout << endl << "-----------------------------------------" << endl;
	}
}

int main()
{
	bridge = new Bridge();
	TaxoPattern *p1, *p2, *p3;
	p1 = bridge->getKbSchema(bridge->kb->getConceptId("wikicategory_Argentine_expatriates_in_Austria"),
			bridge->kb->getRelationId("playsFor"), true);
	p2 = bridge->getKbSchema(bridge->kb->getConceptId("wikicategory_La_Liga_footballers"),
			bridge->kb->getRelationId("playsFor"), true);
	p3 = bridge->colPattern[bridge->corpus->getTableByDataId(1356).id][2];
	bridge->printPattern(p3);

	depthVector sim1 = Matcher::dVectorJaccard(bridge->kb, p1, p3);
	depthVector sim2 = Matcher::dVectorJaccard(bridge->kb, p2, p3);
	cout << endl;
	cout << "Similarity for wikicategory_Argentine_expatriates_in_Austria : " << endl;
	cout << '\t';
	for (int i = (int) sim1.w.size() - 1; i >= 0; i --)
		cout << sim1.w[i] << " ";
	cout << endl << endl;
	cout << "Similarity for wikicategory_La_Liga_footballers : " << endl;
	cout << '\t';
	for (int i = (int) sim2.w.size() - 1; i >= 0; i --)
		cout << sim2.w[i] << " ";
	cout << endl << endl;

//	Experiment* experiment = new Experiment(bridge);
//	experiment->runAllExp();
	interactiveQuery();

	return 0;
}
