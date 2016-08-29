#include "Bridge.h"
#include "Matcher.h"
#include <set>
#include <map>
#include <vector>
#include <string>
#include <iomanip>
#include <utility>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <sys/time.h>
using namespace std;

Bridge::Bridge()
{
	cout << "Initializing Bridge!" << endl;

	//cache clear
	clearCache();

	struct timeval t1, t2;

	//KB
	gettimeofday(&t1, NULL);
	kb = new YAGO();
	gettimeofday(&t2, NULL);
	cout << "KB Initialization time: " << t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) / 1000000.0 << endl;

	//Corpus
	gettimeofday(&t1, NULL);
	corpus = new WWT();
	gettimeofday(&t2, NULL);
	cout << "Corpus Initialization time: " << t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) / 1000000.0 << endl;

	//debug file
	debug.open("debug.txt");

	//initialize match result
	gettimeofday(&t1, NULL);
	initMatch();
	initMatchingRates();
	gettimeofday(&t2, NULL);
	cout << "Matcher Initialization time: " << t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) / 1000000.0 << endl;

	//initialize KB entity patterns
	gettimeofday(&t1, NULL);
	initEntityPattern();
	gettimeofday(&t2, NULL);
	cout << "entityPattern Initialization time: " << t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) / 1000000.0 << endl;

	//initialize kb schema
	gettimeofday(&t1, NULL);
	initKbSchema();
	gettimeofday(&t2, NULL);
	cout << "schemaPattern Initialization time: " << t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) / 1000000.0 << endl;

	//initialize cell taxo patterns
	gettimeofday(&t1, NULL);
	initCellPattern();
	gettimeofday(&t2, NULL);
	cout << "cellPattern Initialization time: " << t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) / 1000000.0 << endl;

	//For Katara: initialize coherence scores
	gettimeofday(&t1, NULL);
	initCoherenceScores();
	gettimeofday(&t2, NULL);
	cout << "Coherence score Initialization time: " << t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) / 1000000.0 << endl;
}

void Bridge::clearCache()
{
	labelCache.clear();
}

void Bridge::initMatch()
{
	cout << "Initializing match!!!!" << endl;
	//Match FileName
	string matchFileName = "../../../data/fuzzy/result_jaccard_0.8_0.8.txt";
	ifstream matchFile(matchFileName);

	string s;
	int totalCell = corpus->countCell();

	matches.clear();
	matches.resize(totalCell + 1);
	for (int i = 1; i <= totalCell; i ++)
		matches[i].clear();

	while (getline(matchFile, s))
	{
		//line 1
		stringstream sin(s);
		double sim;
		int entityId, cellId;

		sin >> sim >> entityId >> cellId;
		entityId ++;
		cellId ++;
		matches[cellId].emplace_back(entityId, sim);

		//line 2~4
		for (int i = 1; i <= 3; i ++)
			getline(matchFile, s);
	}

	//special check for exact matches
	for (int i = 1; i <= totalCell; i ++)
	{
		string curValue = corpus->getCell(i).value;
		bool hasExactMatch = false;
		int exactEntity = -1;
		for (auto kv : matches[i])
			if (kv.second == 1.0)
			{
				hasExactMatch = true;
				exactEntity = kv.first;
			}
		if (hasExactMatch)
		{
			matches[i].clear();
			matches[i].emplace_back(exactEntity, 1.0);
		}
	}
}

void Bridge::initMatchingRates()
{
	colMR.clear();
	tableMR.clear();
	tableMR.push_back(0);
	int nTable = corpus->countTable();

	for (int i = 1; i <= nTable; i ++)
	{
		Table *curTable = corpus->getTable(i);
		int tid = curTable->table_id;
		int nRow = curTable->nRow;
		int nCol = curTable->nCol;
		colMR[tid] = vector<double>(nCol);
		tableMR.push_back(0);

		for (int y = 0; y < nCol; y ++)
		{
			colMR[tid][y] = getNumLuckyCells(curTable, y) / nRow;
			tableMR[i] += colMR[tid][y];
		}
		tableMR[i] /= nCol;
	}
}

void Bridge::initEntityPattern()
{
	int totalEntity = kb->countEntity();
	entPattern.resize(totalEntity + 1);

	for (int i = 1; i <= totalEntity; i ++)
	{
		int totalBelong = kb->getBelongCount(i);

		entPattern[i] = new TaxoPattern();
		entPattern[i]->e[i] = 1.0;
		entPattern[i]->numEntity = 1.0;
		for (int j = 0; j < totalBelong; j ++)
		{
			int curConcept = kb->getBelongConcept(i, j);
			while (1)
			{
				entPattern[i]->c[curConcept] = 1.0;
				if (kb->getPreCount(curConcept) == 0)
					break;
				curConcept = kb->getPreNode(curConcept, 0);
			}
		}
	}
}

void Bridge::initKbSchema()
{
	cout << "Initiliazing kbSchema!!!" << endl;

	int totalConcept = kb->countConcept();
	int totalEntity = kb->countEntity();

	//make the entity schema
	entSchema.resize(totalEntity + 1);
	for (int i = 1; i <= totalEntity; i ++)
		entSchema[i].clear();

	for (int i = 1; i <= totalEntity; i ++)
	{
		int totalRelatedFact = kb->getFactCount(i);
		for (int j = 0; j < totalRelatedFact; j ++)
		{
			pair<int, int> curPair = kb->getFactPair(i, j);
			int curRelation = curPair.first;
			int k = curPair.second;

			//i <curRelation> k
			if (! entSchema[i].count(curRelation))
				entSchema[i][curRelation] = new TaxoPattern();
			entSchema[i][curRelation]->add(entPattern[k]);
		}
		for (auto &kv : entSchema[i])
			kv.second->numEntity --;
	}

	//make the concept Schemas
	conSchema.resize(totalConcept + 1);
	for (int i = 1; i <= totalConcept; i ++)
	{
		conSchema[i].clear();
		unordered_set<int> &entities = kb->getRecursivePossessEntities(i);
		for (int e : entities)
			for (auto kv : entSchema[e])
			{
				if (! conSchema[i].count(kv.first))
					conSchema[i][kv.first] = new TaxoPattern();
				conSchema[i][kv.first]->add(kv.second);
			}
		for (auto &kv : conSchema[i])
			kv.second->numEntity += entities.size();
	}
	//add back
	for (int i = 1; i <= totalEntity; i ++)
		for (auto &kv : entSchema[i])
			kv.second->numEntity ++;
}

void Bridge::initCellPattern()
{
	cout << "Initializing cell taxonomy patterns!!!" << endl;

	int totalCell = corpus->countCell();
	int totalTable = corpus->countTable();

	//Initialize container
	cellPattern.clear();
	cellPattern.resize(totalCell + 1);
	colPattern.clear();
	colPattern.resize(totalTable + 1);

	//make patterns for lucky cells
	for (int i = 1; i <= totalCell; i ++)
	{
		if (matches[i].size() == 0)
			continue;

		cellPattern[i] = new TaxoPattern();
		double wt = (matches[i][0].second < 1.0 ? Param::WT_SEMILUCKY : 1.0);
		for (int j = 0; j < (int) matches[i].size(); j ++)
		{
			int curEntity = matches[i][j].first;
			TaxoPattern curPattern = *entPattern[curEntity];
			curPattern.mult(wt / (double) matches[i].size());
			cellPattern[i]->add(&curPattern);
		}
	}

	//make patterns for unlucky cells
	for (int i = 1; i <= totalTable; i ++)
	{
		Table *curTable = corpus->getTable(i);
		int nRow = curTable->nRow;
		int nCol = curTable->nCol;
		colPattern[i].clear();

		for (int y = 0; y < nCol; y ++)
		{
			colPattern[i].push_back(new TaxoPattern());
			TaxoPattern *curColPattern = colPattern[i][y];

			for (int x = 0; x < nRow; x ++)
			{
				Cell cur = curTable->cells[x][y];
				if (matches[cur.id].size() == 0)
					continue;
				curColPattern->add(cellPattern[cur.id]);
			}

			for (int x = 0; x < nRow; x ++)
			{
				Cell cur = curTable->cells[x][y];
				if (matches[cur.id].size())
					continue;
				cellPattern[cur.id] = curColPattern;
			}
		}
	}
}

vector<pair<int, double>>& Bridge::getMatch(int cellId)
{
	return matches[cellId];
}

void Bridge::traverse()
{
	int cur = kb->getRoot();
	vector<int> q; q.clear();
	q.push_back(cur);

	while (1)
	{
		cur = q[q.size() - 1];
		cout << "We are at : " << cur << " " << kb->getConcept(cur) << endl
			<< "Input your operation: " << endl;

		int x, tmp;
		string s;
		cin >> x;

		switch (x)
		{
			case 0 :
				return ;
			case 1 :
				//print out the number of successors
				cout << "The number of successors: " << endl << kb->getSucCount(cur) << endl;
				break;
			case 2 :
				//print all the successors
				cout << "The successors are: " << endl;
				tmp = kb->getSucCount(cur);
				for (int i = 0; i < tmp; i ++)
					cout << kb->getConcept(kb->getSucNode(cur, i)) << "     ";
				cout << endl;
				break;
			case 3 :
				//Go upwards
				if (q.size() == 1)
					cout << "We cannot go upward any more!" << endl;
				else
					q.pop_back();
				break;
			case 4 :
				cin >> s;
				q.push_back(kb->getConceptId(s));
				break;
			case 5 :
				cout << (tmp = kb->getPossessCount(cur)) <<
					" entities in this concept! Sample entities are: " << endl;
				for (int i = 0; i < min(tmp, 10); i ++)
					cout << "  " << kb->getEntity(kb->getPossessEntity(cur, i)) << endl;
				break;
			case 6 :
				tmp = 0;
				for (int i = 0; i < kb->getPossessCount(cur); i ++)
				{
					int j = kb->getPossessEntity(cur, i);
					tmp += kb->getFactCount(j);
				}
				cout << "Number of facts related to this concept: " << endl << tmp << endl;
				break;
			case 7 :
				cout << "Size of the schema of this node: " << endl << "  "
					<< (conSchema[cur]).size() << endl;
				break;
			case 8 :
				cout << "Sample Relations are: " << endl << endl;
				for (auto kv1 : conSchema[cur])
				{
					cout << kb->getRelation(kv1.first) << " : " << endl << "    ";
					int cnt = 0;
					for (auto kv2 : (kv1.second)->c)
					{
						cout << kb->getConcept(kv2.first) << "    ";
						cnt ++;
						if (cnt == 20)
							break;
					}
					cout << endl;
					cout << endl;
				}
				cout << endl;
				break;
		}
		cout << "-------------------------------------" << endl << endl;
	}
}
