#include <map>
#include <set>
#include <ctime>
#include <vector>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unordered_set>
using namespace std;

const string dir = "../../../data/KB/kb_sqlite.sql";

string make(string s)
{
	string ans = "";
	for (int i = 0; i < s.size(); i ++)
		if (s[i] != '\'')
			ans += s[i];
		else
			ans += "\'\'";
	return ans;
}

void genYagoEntityTable()
{
	ifstream fin("../../../data/KB/Entities.txt");
	ofstream fout;
	fout.open(dir.c_str(), ofstream::out | ofstream::app);
	fout << "begin;" << endl << endl;

	string value;
	int cur = 0;

	while (fin >> value)
		fout << "INSERT INTO `yago_entity` VALUES ("
		     << ++ cur << "," << "'" << make(value) << "');" << endl;
//		fout << ++ cur << '\t' << value << endl;
	fout << endl;
	fin.close();
	fout.close();
}

void genFuzzyMatchTable()
{
	ifstream fin2("../../../data/fuzzy/result_jaccard_0.8_0.8.txt");
	ifstream fin1("../../../data/Table/wwt_id");
	ofstream fout;
	fout.open(dir.c_str(), ofstream::out | ofstream::app);

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

		fout << "INSERT INTO `fuzzy_match` VALUES ("
		     << ++ id << "," << row[cellID] << "," << col[cellID]
		     << "," << table_id[cellID] << "," << entityID << ");" << endl;
//		fout << ++id << '\t' << row[cellID] << '\t' << col[cellID] << '\t' << table_id[cellID] << '\t' << entityID << endl;

		for (int i = 1; i <= 3; i ++)
			getline(fin2, s);
	}
	fout << endl;

	fin1.close();
	fin2.close();
	fout.close();
}

void genTypeTable()
{
	ifstream fin1("../../../data/KB/Types.txt");
	ofstream fout;
	fout.open(dir.c_str(), ofstream::out | ofstream::app);

	int x, y, id = 0;
	while (fin1 >> x >> y)
		fout << "INSERT INTO `yago_type` VALUES ("
		     << ++ id << "," << x << "," << y << ");" << endl;
//		fout << ++ id << '\t' << x << '\t' << y << endl;
	fout << endl;

	fin1.close();
	fout.close();
}

void genConceptTable()
{
	ifstream fin1("../../../data/KB/Concepts.txt");
	ofstream fout;
	fout.open(dir.c_str(), ofstream::out | ofstream::app);

	string value;
	int id = 0;

	while (getline(fin1, value))
		fout << "INSERT INTO `yago_concept` VALUES ("
		     << ++ id << "," << "'" << make(value) << "');" << endl;
//		fout << ++ id << '\t' << value << endl;
	fout << endl << endl << "commit;" << endl;

	fin1.close();
	fout.close();
}

void genColRelationResultTable()
{
	ifstream fin1("../../../data/Result/colRelation/colRelation_way2_1e57.txt");
	ofstream fout;
	fout.open(dir.c_str(), ofstream::out | ofstream::app);

	vector<string> res;
	string s;
	while (getline(fin1, s))
	{
		for (int i = 0; i < s.size(); i ++)
			if (s[i] == ' ')
				s[i] = '\t';
			else if (s[i] == '*')
				break;
		res.push_back(s);
	}

	//randomly select 100 columns
	srand(time(0));
	unordered_set<int> selected;
	for (int lp = 0; lp < 100; lp ++)
	{
		int x;
		while (1)
		{
			x = rand() % ((int) res.size());
			if (! selected.count(x))
				break;
		}
		selected.insert(x);

		fout << "INSERT INTO `col_relation` VALUES ("
		     << lp + 1 << "," << "'" << make(res[x]) << "'"
		     << "," << 0 << ");" << endl;
//		fout << lp + 1 << '\t' << res[x] << '\t' << 0 << endl;
	}
	fout << endl;
	fout.close();
}

void genRecConceptResultTable()
{
	ifstream fin1("../../../data/Result/recConcept/recConcept_top5_dVector.txt");
	ofstream fout;
	fout.open(dir.c_str(), ofstream::out | ofstream::app);

	vector<string> res;
	string s;
	vector<pair<int, int>> uni, v;

	while (getline(fin1, s))
	{
		for (int i = 0; i < s.size(); i ++)
			if (s[i] == ' ')
				s[i] = '\t';
		res.push_back(s);

		stringstream sin(s);
		int table_id, row_id, rank;
		sin >> table_id >> row_id >> rank;

		v.push_back(make_pair(table_id, row_id));
		if (rank == 0)
			uni.push_back(make_pair(table_id, row_id));
	}

	set<int> S; S.clear();
	vector<int> selected;
	for (int lp = 1; lp <= 100; lp ++)
	{
		int x;
		while (1)
		{
			x = rand() % ((int) uni.size());
			if (! S.count(x))
				break;
		}
		S.insert(x);
		selected.push_back(x);
	}

	int tot = 0;
	for (int lp = 1; lp <= 100; lp ++)
	{
		int id = selected[lp - 1];
		for (int i = 0; i < res.size(); i ++)
			if (v[i] == uni[id])
				fout << "INSERT INTO `rec_concept` VALUES ("
				     << ++ tot << "," << lp << ","
				     << "'" << make(res[i]) << "'" << ","
				     << -1 << ");" <<  endl;
//				fout << ++ tot << '\t' << lp << '\t' << res[i]
//					<< '\t' << -1 << endl;
	}
	fout << endl;
	fout.close();
}

int main()
{
	genYagoEntityTable();
//	genFuzzyMatchTable();
	genTypeTable();
	genConceptTable();
//	genColRelationResultTable();
//	genRecConceptResultTable();
	return 0;
}
