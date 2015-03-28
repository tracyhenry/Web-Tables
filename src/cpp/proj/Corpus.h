#ifndef CORPUS__H
#define CORPUS__H

#include <map>
#include <vector>
#include <string>


struct Cell
{
	Cell() {id = 0; }
	Cell(int, int, int, int, std::string);

	int row, col, table_id, id;
	std::string value;

	int operator < (const Cell &o) const
	{
		return (table_id < o.table_id ||
			table_id == o.table_id && row < o.row ||
			table_id == o.table_id && row == o.row && col < o.col);
	}
};

struct Table
{
	Table() {cells.clear(); }
	std::vector<std::vector<Cell>> cells;
	int nRow, nCol, table_id, entityCol;

	int operator < (const Table &o) const
	{
		return table_id < o.table_id;
	}
};

class Corpus
{
public:
	Corpus();
	int countMultiColumnTable();

	//public auxiliary functions
	int countCell();
	int countTable();
	Cell getCell(int);		//by cell id
	Table getTable(int);		//by table id

protected:
	//constants
	const std::string dirPath;
	const std::string delim;

	//cells and tables
	std::vector<Cell> allCells;
	std::vector<Table> allTables;
	int nCell;
	int nTable;
};

class WWT : public Corpus
{
public :
	WWT();
	Table getTableByDataId(int);

private:

	//table_id (in original dataset) --> id (in allTables[])
	std::map<int, int> idMap;
};

#endif
