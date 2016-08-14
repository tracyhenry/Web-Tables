#include "Bridge.h"
#include "Matcher.h"
#include "Param.h"
#include <queue>
#include <iostream>
using namespace std;

void Bridge::enrichKB()
{
	naiveFactTriple();
	naiveTypePair();
}

void Bridge::naiveFactTriple()
{
	string resultFileName = "../../../data/Result/enrichment/naiveFactTriple.txt";
	ofstream fout(resultFileName.c_str());

	//col relation array
	int nTable = corpus->countTable();
	vector<Table> tables;
	vector<vector<int>> labels;
	for (int i = 1; i <= nTable; i ++)
	{
		if (i % 200 == 0)
			cout << i << " tables have been processed..." << endl;
		tables.push_back(corpus->getTable(i));
		labels.push_back(findColConceptAndRelation(corpus->getTable(i).table_id, false));
	}

	//sort by sim
	priority_queue<pair<double, pair<int, int>>> h;
	for (int i = 1; i <= nTable; i ++)
	{
		int nCol = tables[i].nCol;
		int entityCol = tables[i].entityCol;

		for (int j = 0; j < nCol; j ++)
		{
			if (j == entityCol) continue;
			if (labels[i][j + nCol] == -1)
				continue;

			double sim = 0;
			TaxoPattern *p1, *p2;
			int rel = labels[i][j + nCol];
			int reverseRel = kb->getReverseRelationId(rel);

			if (labels[i][entityCol] != -1)
			{
				p1 = conSchema[labels[i][entityCol]][rel];
				p2 = colPattern[i][j];
				sim += Matcher::patternSim(kb, p1, p2, Param::colConceptSim);
			}
			if (labels[i][j] != -1)
			{
				p1 = conSchema[labels[i][j]][reverseRel];
				p2 = colPattern[i][entityCol];
				sim += Matcher::patternSim(kb, p1, p2, Param::colConceptSim);
			}

			h.push(make_pair(sim, make_pair(i, j)));
		}
	}

	//generate enrichments
//	vector<string> newFactTriples;
	while (! h.empty())
	{
		auto top = h.top();
		h.pop();

		double sim = top.first;
		int i = top.second.first;
		int j = top.second.second;
		int nRow = tables[i].nRow;
		int nCol = tables[i].nCol;
		int rel = labels[i][j + nCol];
		int entityCol = tables[i].entityCol;

		fout << i << " " << j << " : " << sim << endl;
		for (int r = 0; r < nRow; r ++)
		{
			auto m1 = matches[tables[i].cells[r][entityCol].id];
			auto m2 = matches[tables[i].cells[r][j].id];

			bool existInKB = false;
			if (m1.size() && m2.size())
				if (m1[0].second == 1.0 && m2[0].second == 1.0)
				{
					int e1 = m1[0].first;
					int e2 = m2[0].first;
					int factCount = kb->getFactCount(e1);
					for (int k = 0; k < factCount; k ++)
					{
						auto cp = kb->getFactPair(e1, k);
						if (cp.first == rel && cp.second == e2)
						{
							existInKB = true;
							break;
						}
					}
				}

			string cur = "";
			cur += tables[i].cells[r][entityCol].value;
			cur += " ";
			cur += kb->getRelation(rel);
			cur += " ";
			cur += tables[i].cells[r][j].value;

			fout << '\t' << cur << " " << ! existInKB << endl;
		}
	}
}

void Bridge::naiveTypePair()
{
}
