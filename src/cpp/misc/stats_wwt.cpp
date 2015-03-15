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
	int nRow, nCol, table_id;
};


vector<Cell> allCells;
vector<Table> allTables;

int nCell = 0;
int nTable = 0;

int main()
{
	ifstream fin1("../../../data/Table/wwt_id");
	ifstream fin2("../../../data/Table/wwt_value");
	ifstream fin3("../../../data/fuzzy/result_0.8_0.8.txt");
	freopen("../../../data/fuzzy/stats_0.8_0.8.txt", "a", stdout);

	allCells.clear();
	allCells.push_back(Cell());

	//File 1
	int r, c, tid;
	while (fin1 >> r >> c >> tid)
	{
		Cell cur;
		cur.row = r, cur.col = c, cur.table_id = tid;
		cur.id = ++ nCell;
		cur.matched = 0;
		allCells.push_back(cur);
	}
//	cout << "Read file 1!" << endl;

	//File 2
	string value;
	for (int i = 1; i <= nCell; i ++)
	{
		getline(fin2, value);
		allCells[i].value = value;
	}
//	cout << "Read file 2!" << endl;

	//File 3
	string s;
	while (getline(fin3, s))
	{
		//line 1
		stringstream sin1(s);
		double sim;
		int entityID, cellID;

		sin1 >> sim >> entityID >> cellID;
		allCells[cellID].matched ++;
	}
//	cout << "Read file 3!" << endl;

	//make tables
	allTables.clear();
	sort(allCells.begin(), allCells.end());
	cout << "Sorted table! The number of cells: " << endl << "      " << nCell << endl;

	for (int i = 1; i <= nCell; i ++)
		if (i == 1 || allCells[i].table_id != allCells[i - 1].table_id)
		{
//			cout << i << " " << allTables.size() << endl;
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

			allTables.push_back(cur);
		}

	//make some stats

	nTable = allTables.size();
	cout << "Total number of tables in WWT: " << endl << "      " << nTable << endl;

	int nManyCol = 0;
	for (int i = 0; i < nTable; i ++)
		if (allTables[i].nCol >= 4)
			nManyCol ++;

	cout << "Total number of tables that have more than 4 columns: " << endl << "      " << nManyCol << endl;

	int nManyLucky = 0;
	for (int i = 0; i < nTable; i ++)
	{
		int nLucky = 0;
		int total = allTables[i].nRow * allTables[i].nCol;
		for (int x = 0; x < allTables[i].nRow; x ++)
			for (int y = 0; y < allTables[i].nCol; y ++)
				if (allTables[i].cells[x][y].matched)
					nLucky ++;

		if (nLucky * 2 >= total)
			nManyLucky ++;
	}

	cout << "Total number of tables that have more than 50% lucky cells: " << endl << "      " << nManyLucky << endl;

	return 0;
}
