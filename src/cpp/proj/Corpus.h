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
};

class Corpus
{
public:
	Corpus();
	int countMultiColumnTable();

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
};

#endif
