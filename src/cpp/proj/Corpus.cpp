#include "Corpus.h"
#include <map>
#include <set>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
using namespace std;


Cell::Cell(int r, int c, int tid, int cid, string v)
{
	row = r, col = c, table_id = tid, id = cid;
	value = v;
}

Corpus::Corpus() :
	dirPath("../../../data/Table/"), delim("/")
{
	nCell = nTable = 0;
	allCells.clear();
	allTables.clear();
}

int Corpus::countMultiColumnTable()
{
	int ans = 0;
	// Get the number of tables that have more than 4 columns
	for (int i = 1; i <= nTable; i ++)
		if (allTables[i].nCol >= 4)
			ans ++;
	return ans;
}

int Corpus::countCell()
{
	return nCell;
}

int Corpus::countTable()
{
	return nTable;
}

Cell Corpus::getCell(int id)
{
	return allCells[id];
}

Table Corpus::getTable(int id)
{
	return allTables[id];
}

Table WWT::getTableByDataId(int id)
{
	return allTables[idMap[id]];
}

WWT::WWT()
{
	cout << "Initializing WWT dataset!" << endl;

	string idFileName = dirPath + "wwt_id";
	string valueFileName = dirPath + "wwt_value";
	string isEntityFileName = dirPath + "wwt_isentity";

	ifstream idFile(idFileName.c_str());
	ifstream valueFile(valueFileName.c_str());
	ifstream isEntityFile(isEntityFileName.c_str());

	allCells.push_back(Cell());

	//id File
	int r, c, tid;
	while (idFile >> r >> c >> tid)
		allCells.push_back(Cell(r, c, tid, ++ nCell, ""));

	//value File
	string value;
	for (int i = 1; i <= nCell; i ++)
	{
		getline(valueFile, value);
		allCells[i].value = value;
	}

	//isEntity File
	int isentity;
	map<int, int> entityCol; entityCol.clear();
	while (isEntityFile >> c >> isentity >> tid)
		if (isentity)
			entityCol[tid] = c;

	//make allTables
	cout << "Making tables....." << endl;
	sort(allCells.begin() + 1, allCells.end());
	allTables.push_back(Table());

	for (int i = 1; i <= nCell; i ++)
		if (i == 1 || allCells[i].table_id != allCells[i - 1].table_id)
		{
			Table cur;
			cur.table_id = allCells[i].table_id;
			cur.cells.clear();
			cur.nRow = 0;
			cur.nCol = 0;

			//Cells
			for (int j = i; j <= nCell; j ++)
			{
				if (allCells[j].table_id != allCells[i].table_id)
					break;
				int x = allCells[j].row;
				int y = allCells[j].col;

				if (x + 1 > (int) cur.cells.size())
					cur.cells.resize(x + 1);
				if (y + 1 > (int) cur.cells[x].size())
					cur.cells[x].resize(y + 1);
				cur.cells[x][y] = allCells[j];
			}

			cur.nRow = cur.cells.size();
			cur.nCol = (cur.nRow ? cur.cells[0].size() : 0);

			//entity column
			if (entityCol.count(cur.table_id))
				cur.entityCol = entityCol[cur.table_id];
			else
				cur.entityCol = -1;

			allTables.push_back(cur);
		}

	sort(allTables.begin() + 1, allTables.end());
	nTable = allTables.size() - 1;

	//idMap
	idMap.clear();
	for (int i = 1; i <= nTable; i ++)
		idMap[allTables[i].table_id] = i;
}
