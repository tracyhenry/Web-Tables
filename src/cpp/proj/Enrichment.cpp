#include "Bridge.h"
#include "Matcher.h"
#include "Param.h"
#include <queue>
#include <iostream>
using namespace std;

void Bridge::enrichKB()
{
	genFactTriple();
	genAttrTypePair();
	genEntityTypePair();
}

void Bridge::genFactTriple()
{
	string resultFileName = "../../../data/Result/enrichment/factTriple.txt";
	ofstream fout(resultFileName.c_str());

	//col relation array
	int nTable = corpus->countTable();
	vector<Table *> tables(1);
	vector<vector<int>> labels(1);
	for (int i = 1; i <= nTable; i ++)
	{
		if (i % 500 == 0)
			cout << i << " tables have been processed..." << endl;
		tables.push_back(corpus->getTable(i));
		labels.push_back(findColConceptAndRelation(corpus->getTable(i)->table_id, false));
	}

	//sort by sim
	priority_queue<pair<double, pair<int, int>>> h;
	for (int i = 1; i <= nTable; i ++)
	{
		int nCol = tables[i]->nCol;
		int entityCol = tables[i]->entityCol;

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
				if (conSchema[labels[i][entityCol]].count(rel))
					p1 = conSchema[labels[i][entityCol]][rel];
				else
					p1 = NULL;
				p2 = colPattern[i][j];
				sim += Matcher::patternSim(kb, p1, p2, Param::colConceptSim);
			}
			if (labels[i][j] != -1)
			{
				if (conSchema[labels[i][j]].count(reverseRel))
					p1 = conSchema[labels[i][j]][reverseRel];
				else
					p1 = NULL;
				p2 = colPattern[i][entityCol];
				sim += Matcher::patternSim(kb, p1, p2, Param::colConceptSim);
			}

			double matchingRate = tableMR[i];
			h.push(make_pair(sim * matchingRate, make_pair(i, j)));
		}
	}

	//generate enrichments
	while (! h.empty())
	{
		auto top = h.top();
		h.pop();

		double sim = top.first;
		int i = top.second.first;
		int j = top.second.second;
		int nRow = tables[i]->nRow;
		int nCol = tables[i]->nCol;
		int rel = labels[i][j + nCol];
		int entityCol = tables[i]->entityCol;

		fout << "# " << tables[i]->table_id << " " << j << " " << sim << endl;
		for (int r = 0; r < nRow; r ++)
		{
			auto m1 = matches[tables[i]->cells[r][entityCol].id];
			auto m2 = matches[tables[i]->cells[r][j].id];

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
			if (existInKB)
				continue;

			string cur = "";
			cur += to_string(r);
			cur += "________";
			cur += tables[i]->cells[r][entityCol].value;
			cur += "________";
			cur += kb->getRelation(rel);
			cur += "________";
			cur += tables[i]->cells[r][j].value;

			fout << cur << endl;
		}
		fout << endl;
	}
	fout.close();
}

void Bridge::genAttrTypePair()
{
	string resultFileName = "../../../data/Result/enrichment/attributeTypepair.txt";
	ofstream fout(resultFileName.c_str());

	//col relation array
	int nTable = corpus->countTable();
	vector<Table *> tables(1);
	vector<vector<int>> labels(1);
	for (int i = 1; i <= nTable; i ++)
	{
		if (i % 500 == 0)
			cout << i << " tables have been processed..." << endl;
		tables.push_back(corpus->getTable(i));
		labels.push_back(findColConceptAndRelation(corpus->getTable(i)->table_id, false));
	}

	//sort by sim
	priority_queue<pair<double, pair<int, int>>> h;
	for (int i = 1; i <= nTable; i ++)
	{
		int nCol = tables[i]->nCol;
		int entityCol = tables[i]->entityCol;

		for (int j = 0; j < nCol; j ++)
		{
			if (j == entityCol) continue;
			if (labels[i][j] == -1)
				continue;

			double sim = 0;
			if (labels[i][j + nCol] != -1)
			{
				TaxoPattern *p1, *p2;
				int rel = kb->getReverseRelationId(labels[i][j + nCol]);
				if (conSchema[labels[i][j]].count(rel))
					p1 = conSchema[labels[i][j]][rel];
				else
					p1 = NULL;
				p2 = colPattern[i][entityCol];
				sim += Matcher::patternSim(kb, p1, p2, Param::colConceptSim);
			}

			double matchingRate = colMR[tables[i]->table_id][j];
			h.push(make_pair(sim * matchingRate, make_pair(i, j)));
		}
	}

	//generate enrichments
	while (! h.empty())
	{
		auto top = h.top();
		h.pop();

		double sim = top.first;
		int i = top.second.first;
		int j = top.second.second;
		int nRow = tables[i]->nRow;
		int conceptLabel = labels[i][j];

		fout << "# " << tables[i]->table_id << " " << j << " " << sim << endl;
		for (int r = 0; r < nRow; r ++)
		{
			auto m = matches[tables[i]->cells[r][j].id];
			bool existInKB = false;
			if (m.size() && m[0].second == 1.0)
			{
				int e = m[0].first;
				int belongCount = kb->getBelongCount(e);
				for (int k = 0; k < belongCount; k ++)
				{
					int conceptId = kb->getBelongConcept(e, k);
					if (kb->isDescendant(conceptId, conceptLabel))
					{
						existInKB = true;
						break;
					}
				}
			}
			if (existInKB)
				continue;

			string cur = "";
			cur += to_string(r);
			cur += "________";
			cur += tables[i]->cells[r][j].value;
			cur += "________";
			cur += kb->getConcept(conceptLabel);
			fout << cur << endl;
		}
		fout << endl;
	}
	fout.close();
}

void Bridge::genEntityTypePair()
{
	string resultFileName = "../../../data/Result/enrichment/entityTypepair.txt";
	ofstream fout(resultFileName.c_str());

	//tables
	int nTable = corpus->countTable();
	vector<Table *> tables(1);
	for (int i = 1; i <= nTable; i ++)
		tables.push_back(corpus->getTable(i));

	//records
	vector<pair<int, int>> records;
	for (int i = 1; i <= nTable; i ++)
	{
		int nRow = tables[i]->nRow;
		for (int r = 0; r < nRow; r ++)
			records.emplace_back(i, r);
	}

	//score heap: <sigma, <recId, conceptId>>
	priority_queue<pair<double, pair<int, int>>> h;

	//current k for each record
	vector<int> curK(records.size());

	//label answer
	vector<vector<int>> labels(records.size());

	//initialize, insert top-1 of each record into h
	cout << "Number of records: " << records.size() << endl;
	for (int recId = 0; recId < (int) records.size(); recId ++)
	{
		if (recId % 500 == 0)
			cout << recId << " records have been processed." << endl;
		int i = records[recId].first;
		int r = records[recId].second;
		labels[recId] = fastFindRecordConcept(tables[i]->table_id, r, Param::MAXK, false, false);
		if ((int) labels[recId].size() < 1)
			continue;
		int conceptId = labels[recId][0];
		double sigmaValue = sigma(conceptId, tables[i]->table_id, r);
		double luckyRate = tableMR[i];
		h.push(make_pair(sigmaValue * luckyRate, make_pair(recId, conceptId)));
		curK[recId] = 1;
	}

	//top-k
	int nEnrichment = 0;
	while (! h.empty())
	{
		if (nEnrichment % 500 == 0)
			cout << h.size() << " " << nEnrichment << endl;

		auto cp = h.top();
		h.pop();

		double sigmaTop = cp.first;
		int recId = cp.second.first;
		int conceptId = cp.second.second;
		int i = records[recId].first;
		int r = records[recId].second;
		int entityCol = tables[i]->entityCol;
		if (entityCol == -1)
			continue;

		//check if it's an enrichment
		auto m = matches[tables[i]->cells[r][entityCol].id];

		bool existInKB = false;
		if (m.size() && m[0].second == 1.0)
		{
			int e = m[0].first;
			int belongCount = kb->getBelongCount(e);
			for (int k = 0; k < belongCount; k ++)
			{
				int curConcept = kb->getBelongConcept(e, k);
				if (curConcept == conceptId)
				{
					existInKB = true;
					break;
				}
			}
		}

		//output
		if (! existInKB)
		{
			string cur = "";
			cur += tables[i]->cells[r][entityCol].value;
			cur += "________";
			cur += kb->getConcept(conceptId);

			fout << "# " << tables[i]->table_id << " " << r << " " << curK[recId] << " : " << sigmaTop << endl;
			fout << cur << endl << endl;

			nEnrichment ++;
			if (nEnrichment == Param::TYPE_PAIR_K)
				break;
		}

		//move onto next
		if (curK[recId] < Param::MAXK)
		{
			int pos = ++ curK[recId];
			if ((int) labels[recId].size() >= pos)
			{
				int curConcept = labels[recId][pos - 1];
				double sigmaValue = sigma(curConcept, tables[i]->table_id, r);
				double luckyRate = tableMR[i];
				h.push(make_pair(sigmaValue * luckyRate / pos, make_pair(recId, curConcept)));
			}
		}
	}
	fout.close();
}
