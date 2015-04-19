#include <map>
#include <vector>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
using namespace std;

void genYagoEntityTable()
{
	ifstream fin("../../../data/KB/Entities.txt");
	ofstream fout("/tmp/yago_entity_db.txt");

	string value;
	int cur = 0;

	while (fin >> value)
		fout << ++ cur << '\t' << value << endl;
	fin.close();
	fout.close();
}

void genFuzzyMatchTable()
{
	ifstream fin2("../../../data/fuzzy/result_jaccard_0.8_0.8.txt");
	ifstream fin1("../../../data/Table/wwt_id");
	ofstream fout("/tmp/fuzzy_db.txt");

	//initializations
	vector<int> row, col, table_id;
	row.clear(), col.clear(), table_id.clear();
	row.push_back(-1);
	col.push_back(-1);
	table_id.push_back(-1);

	//File 1
	int r, c, tid;
	while (fin1 >> r >> c >> tid)
		row.push_back(r), col.push_back(c), table_id.push_back(tid);

	string s;
	int id = 0;
	while (getline(fin2, s))
	{
		//line 1
		stringstream sin1(s);
		double sim;
		int entityID, cellID;
		sin1 >> sim >> entityID >> cellID;

		entityID ++;
		cellID ++;

		fout << ++id << '\t' << row[cellID] << '\t' << col[cellID] << '\t' << table_id[cellID] << '\t' << entityID << endl;

		for (int i = 1; i <= 3; i ++)
			getline(fin2, s);
	}

	fin1.close();
	fin2.close();
	fout.close();
}

void genTypeTable()
{
	ifstream fin1("../../../data/KB/Types.txt");
	ofstream fout("/tmp/types_db.txt");
	int x, y, id = 0;
	while (fin1 >> x >> y)
		fout << ++ id << '\t' << x << '\t' << y << endl;
	fin1.close();
	fout.close();
}

void genConceptTable()
{
	ifstream fin1("../../../data/KB/Concepts.txt");
	ofstream fout("/tmp/concept_db.txt");
	string value;
	int id = 0;

	while (getline(fin1, value))
		fout << ++ id << '\t' << value << endl;
	fin1.close();
	fout.close();
}

int main()
{
	genYagoEntityTable();
	genFuzzyMatchTable();
	genTypeTable();
	genConceptTable();

	return 0;
}
