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
	vector<Table *> tables(1);
	vector<vector<int>> labels(1);
	for (int i = 1; i <= nTable; i ++)
	{
		if (i % 200 == 0)
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
		int nRow = tables[i]->nRow;
		int nCol = tables[i]->nCol;
		int rel = labels[i][j + nCol];
		int entityCol = tables[i]->entityCol;

		fout << tables[i]->table_id << " " << j << " : " << sim << endl;
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
			cur += tables[i]->cells[r][entityCol].value;
			cur += " ";
			cur += kb->getRelation(rel);
			cur += " ";
			cur += tables[i]->cells[r][j].value;

			fout << '\t' << cur << " " << endl;
		}
	}
	fout.close();
}

void Bridge::naiveTypePair()
{
	string resultFileName = "../../../data/Result/enrichment/naiveTypepair.txt";
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

	//initialize, insert top-1 of each record into h
	cout << "Number of records: " << records.size() << endl;
	for (int recId = 0; recId < (int) records.size(); recId ++)
	{
		int i = records[recId].first;
		int r = records[recId].second;
		vector<int> top1 = fastFindRecordConcept(tables[i]->table_id, r, 1, false);
		if ((int) top1.size() < 1)
			continue;
		int conceptId = top1[0];
		double sigmaValue = sigma(conceptId, tables[i]->table_id, r);
		h.push(make_pair(sigmaValue, make_pair(recId, conceptId)));
		curK[recId] = 1;
	}

	//top-k
	int nEnrichment = 0;
	while (! h.empty())
	{
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
		auto m = matches[tables[i]->cells[entityCol][r].id];

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
			cur += " is_an_instance_of ";
			cur += kb->getConcept(conceptId);

			fout << tables[i]->table_id << " " << r << " " << curK[recId] << " : " << sigmaTop << endl;
			fout << '\t' << cur << endl;

			nEnrichment ++;
			if (nEnrichment == Param::TYPE_PAIR_K)
				break;
		}

		//move onto next
		if (curK[recId] < Param::MAXK)
		{
			int pos = ++ curK[recId];
			vector<int> topk = fastFindRecordConcept(tables[i]->table_id, r, pos, false);
			if ((int) topk.size() >= pos)
			{
				int curConcept = topk[pos - 1];
				double sigmaValue = sigma(curConcept, tables[i]->table_id, r);
				h.push(make_pair(sigmaValue / pos, make_pair(recId, curConcept)));
			}
		}
	}
	fout.close();
}
