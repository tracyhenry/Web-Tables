#include <algorithm>
#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>
#include <vector>
#include <map>
#include <set>
using namespace std;

struct Cell
{
	Cell() {}
	int row, col, id, table_id;
	string value;
	int matched;

	int operator < (const Cell &o) const
	{
		return (table_id < o.table_id ||
			table_id == o.table_id && row < o.row ||
			table_id == o.table_id && row == o.row && col < o.col);
	}
};

struct Table
{
	vector<vector<Cell>> cells;
	int nRow, nCol, table_id, entityCol;
};


vector<Cell> allCells;
vector<Table> allTables;
vector<string> E;

int nCell = 0;
int nTable = 0;

int main()
{
	ifstream fin0("../../../data/KB/Entities.txt");
	ifstream fin1("../../../data/Table/wwt_id");
	ifstream fin2("../../../data/Table/wwt_value");
	ifstream fin3("../../../data/Table/wwt_isentity");
	ifstream fin4("../../../data/fuzzy/result_jaccard_0.8_0.8.txt");
	freopen("../../../data/fuzzy/stats_jaccard_0.8_0.8.txt", "a", stdout);

	allCells.clear();
	allCells.push_back(Cell());

	//File 0
	E.clear(); E.push_back("");
	string e;
	while (getline(fin0, e))
		E.push_back(e);
	//File 1
	int r, c, tid;
	while (fin1 >> r >> c >> tid)
	{
		Cell cur;
		cur.row = r, cur.col = c, cur.table_id = tid;
		cur.id = ++ nCell;
		cur.matched = 0;
		cur.value = "";
		allCells.push_back(cur);
	}

	//File 2
	string value;
	for (int i = 1; i <= nCell; i ++)
	{
		getline(fin2, value);
		allCells[i].value = value;
	}

	//File 3
	int isentity;
	map<int, int> entityCol; entityCol.clear();
	while (fin3 >> c >> isentity >> tid)
		if (isentity)
			entityCol[tid] = c;

	//File 4
	string s;
	while (getline(fin4, s))
	{
		//line 1
		stringstream sin1(s);
		double sim;
		int entityID, cellID;

		sin1 >> sim >> entityID >> cellID;
		entityID ++;
		cellID ++;
		allCells[cellID].matched ++;

		//line 2~4
		for (int i = 1; i <= 3; i ++)
			getline(fin4, s);

	}

	//make tables
	allTables.clear();
	sort(allCells.begin(), allCells.end());
//	cout << endl << "Sorted table! The number of cells: " << endl << "      " << nCell << endl;

	for (int i = 1; i <= nCell; i ++)
		if (i == 1 || allCells[i].table_id != allCells[i - 1].table_id)
		{
			Table cur;
			cur.table_id = allCells[i].table_id;
			cur.cells.clear();
			cur.nRow = 0;
			cur.nCol = 0;

			for (int j = i; j <= nCell; j ++)
			{
				if (allCells[j].table_id != allCells[i].table_id)
					break;
				cur.nRow = max(cur.nRow, allCells[j].row + 1);
				cur.nCol = max(cur.nCol, allCells[j].col + 1);
			}

			cur.cells.resize(cur.nRow);
			for (int x = 0; x < cur.nRow; x ++)
				cur.cells[x].resize(cur.nCol);

			for (int j = i; j <= nCell; j ++)
			{
				if (allCells[j].table_id != allCells[i].table_id)
					break;

				int x = allCells[j].row;
				int y = allCells[j].col;

				cur.cells[x][y] = allCells[j];
			}

			//entity column
			if (entityCol.count(cur.table_id))
				cur.entityCol = entityCol[cur.table_id];
			else cur.entityCol = -1;

			allTables.push_back(cur);
		}

	//make some stats

	nTable = allTables.size();
//	cout << endl << "Total number of tables in WWT: " << endl << "      " << nTable << endl;

	int nManyCol = 0;
	for (int i = 0; i < nTable; i ++)
		if (allTables[i].nCol >= 5)
			nManyCol ++;

//	cout << endl << "Total number of tables that have more than 5 columns: " << endl << "      " << nManyCol << endl;

	int nManyLucky = 0;
	for (int i = 0; i < nTable; i ++)
	{
		int nLucky = 0;
		int total = allTables[i].nRow * allTables[i].nCol;
		for (int x = 0; x < allTables[i].nRow; x ++)
			for (int y = 0; y < allTables[i].nCol; y ++)
				if (allTables[i].cells[x][y].matched)
					nLucky ++;

		if (10 * nLucky >= 3 * total)
			nManyLucky ++;
	}

//	cout << endl << "Total number of tables that have more than 30% lucky cells: " << endl << "      " << nManyLucky << endl;

	int maxMatch = 0;
	for (int i = 1; i <= nCell; i ++)
		if (allCells[i].value.size() > 0)
			maxMatch = max(maxMatch, allCells[i].matched);
//	cout << endl << "The maximum number of entities a table cell can be matched to : " << endl << "      " << maxMatch << endl;

	int nEntity = 0;
	int matchedEntity = 0;
	int luckyEntity = 0;
	for (int i = 0; i < nTable; i ++)
	{
		if (allTables[i].entityCol < 0)
			continue;
		int entityCol = allTables[i].entityCol;
		for (int x = 0; x < allTables[i].nRow; x ++)
			if (allTables[i].cells[x][entityCol].matched)
				matchedEntity ++;
		for (int x = 0; x < allTables[i].nRow; x ++)
		{
			int matchedAttr = 0;
			for (int y = 0; y < allTables[i].nCol; y ++)
				if (allTables[i].cells[x][y].matched && y != entityCol)
					matchedAttr ++;
			if (matchedAttr * 5 >= (allTables[i].nCol - 1) * 2)
				luckyEntity ++;
		}
		nEntity += allTables[i].nRow;
	}
//	cout << endl << "The number of total entities in WWT:" << endl << "      " << nEntity << endl;
//	cout << endl << "The number of matched entities in WWT:" << endl << "      " << matchedEntity  << endl;
	cout << endl << "The number of entities that have more than 40% covered attributes: " << endl << "      " << luckyEntity << endl;

	int luckyEntityTable = 0;
	int luckyAttrTable = 0;
	for (int i = 0; i < nTable; i ++)
	{
		if (allTables[i].entityCol < 0)
			continue;
		int entityCol = allTables[i].entityCol;
		int matchedEntity = 0;
		int matchedAttr = 0;
		for (int x = 0; x < allTables[i].nRow; x ++)
			for (int y = 0; y < allTables[i].nCol; y ++)
				if (allTables[i].cells[x][y].matched)
					if (y == entityCol)
						matchedEntity ++;
					else 
						matchedAttr ++;

		if (matchedEntity * 2 >= allTables[i].nRow)
			luckyEntityTable ++;
		if (matchedAttr * 10 >= allTables[i].nRow * (allTables[i].nCol - 1) * 3)
			luckyAttrTable ++;
	}

//	cout << endl << "The number of tables that have more than 50% covered entities: " << endl << "      " << luckyEntityTable << endl;
//	cout << endl << "The number of tables that have more than 30% covered attributes: " << endl << "      " << luckyAttrTable << endl;

	return 0;
}
