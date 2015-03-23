#ifndef CORPUS__H
#define CORPUS__H

#include <map>
#include <vector>
#include <string>


struct Cell
{
	Cell() {}
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
	Table() {}
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
	virtual Cell getCellById(int) = 0;
	virtual Table getTableById(int) = 0;

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
	virtual Cell getCellById(int);
	virtual Table getTableById(int);

private:
	std::map<int, int> idMap;
};

#endif
