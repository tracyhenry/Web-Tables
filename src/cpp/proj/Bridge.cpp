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
#define IterII unordered_map<int, int>::iterator
#define IterIT unordered_map<int, TaxoPattern *>::iterator
using namespace std;

Bridge::Bridge()
{
	cout << "Initializing Bridge!" << endl;

	//KB and Corpus
	kb = new YAGO();
	corpus = new WWT();

	//debug file
	debug.open("debug.txt");

	//initialize match result
	initMatch();

	//initialize KB entity patterns
	initEntityPattern();

	//initialize kb schema
	initKbSchema();

	//initialize cell taxo patterns
	initCellPattern();
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
		matches[cellId].push_back(entityId);

		//line 2~4
		for (int i = 1; i <= 3; i ++)
			getline(matchFile, s);
	}
}

void Bridge::initEntityPattern()
{
	int totalEntity = kb->countEntity();
	entPattern.resize(totalEntity + 1);

	for (int i = 1; i <= totalEntity; i ++)
	{
		entPattern[i] = new TaxoPattern();
		entPattern[i]->e[i] = 1;

		int totalBelong = kb->getBelongCount(i);
		for (int j = 0; j < totalBelong; j ++)
		{
			int curConcept = kb->getBelongConcept(i, j);
			while (1)
			{
				entPattern[i]->c[curConcept] ++;
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
	int root = kb->getRoot();

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
	}

	//make the concept Schema recursively
	conSchema.resize(totalConcept + 1);
	for (int i = 1; i <= totalConcept; i ++)
		conSchema[i].clear();

	cout << "Starting bridge::makeConSchema dfs!" << endl;
	makeConSchema(root);
}

void Bridge::makeConSchema(int curNode)
{
	//instances of curNode
	int totalPossess = kb->getPossessCount(curNode);
	for (int i = 0; i < totalPossess; i ++)
	{
		int curEntity = kb->getPossessEntity(curNode, i);
		unordered_map<int, TaxoPattern *> &curMap = entSchema[curEntity];

		//loop over all relations
		for (IterIT it = curMap.begin(); it != curMap.end(); it ++)
		{
			if (! conSchema[curNode].count(it->first))
				conSchema[curNode][it->first] = new TaxoPattern();
			conSchema[curNode][it->first]->add(it->second);
		}
	}

	//Children
	int totalSuc = kb->getSucCount(curNode);
	for (int i = 0; i < totalSuc; i ++)
	{
		int curSuc = kb->getSucNode(curNode, i);
		makeConSchema(curSuc);

		//loop over all relations
		unordered_map<int, TaxoPattern *> &curMap = conSchema[curSuc];
		for (IterIT it = curMap.begin(); it != curMap.end(); it ++)
		{
			if (! conSchema[curNode].count(it->first))
				conSchema[curNode][it->first] = new TaxoPattern();
			conSchema[curNode][it->first]->add(it->second);
		}
	}
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
		for (int j = 0; j < matches[i].size(); j ++)
		{
			int curEntity = matches[i][j];
			cellPattern[i]->add(entPattern[curEntity]);
		}
	}

	//make patterns for unlucky cells
	for (int i = 1; i <= totalTable; i ++)
	{
		Table curTable = corpus->getTable(i);
		int nRow = curTable.nRow;
		int nCol = curTable.nCol;
		colPattern[i].clear();

		for (int y = 0; y < nCol; y ++)
		{
			colPattern[i].push_back(new TaxoPattern());
			TaxoPattern *curColPattern = colPattern[i][y];
			curColPattern->c[kb->getRoot()] = 1;

			for (int x = 0; x < nRow; x ++)
			{
				Cell cur = curTable.cells[x][y];
				if (matches[cur.id].size() == 0)
					continue;
				curColPattern->add(cellPattern[cur.id]);
			}

			for (int x = 0; x < nRow; x ++)
			{
				Cell cur = curTable.cells[x][y];
				if (matches[cur.id].size())
					continue;
				cellPattern[cur.id] = curColPattern;
			}
		}
	}
}

TaxoPattern *Bridge::getKbSchema(int conceptId, int relationId, bool isDebug)
{
	TaxoPattern *ans = conSchema[conceptId][relationId];
	if (isDebug)
	{
		for (int i = 1; i <= 10; i ++)
			debug << endl;
		debug <<  "-------------------------------------------" << endl;
		debug << conceptId << " " << kb->getConcept(conceptId) << " " <<
			kb->getRelation(relationId) << " : " << endl;
		debug <<  "-------------------------------------------" << endl;

		if (ans != NULL)
			printPattern(ans);
	}
	return ans;
}

TaxoPattern *Bridge::getCellPattern(int cellId, bool isDebug)
{
	TaxoPattern *ans = cellPattern[cellId];
	if (isDebug)
	{
		for (int i = 1; i <= 10; i ++)
			debug << endl;
		debug << "-----------------------------------------" << endl;
		debug << "The pattern of cell id : " << cellId << endl;
		debug << "-----------------------------------------" << endl;

		//sort array
		printPattern(ans);
	}
	return ans;
}

void Bridge::printPattern(TaxoPattern *p)
{
	int testCid = 0;
	unordered_map<int, int> &C = p->c;

	//concepts
	vector<pair<int, int>> tmp; tmp.clear();
	int sum = 1e-9;
	for (IterII it = C.begin(); it != C.end(); it ++)
		if (! testCid || cellPattern[testCid]->c.count(it->first))
			tmp.emplace_back(- it->second, it->first);

	sort(tmp.begin(), tmp.end());
	for (int i = 0; i < tmp.size(); i ++)
		debug << tmp[i].second << " " << kb->getConcept(tmp[i].second)
			<< " : " << - tmp[i].first
			<< " " << kb->getDepth(tmp[i].second) << endl;

	//entities
	unordered_map<int, int> &E = p->e;
	for (IterII it = E.begin(); it != E.end(); it ++)
		if (! testCid || cellPattern[testCid]->e.count(it->first))
			debug << kb->getEntity(it->first) << " : " << it->second << endl;
}

void Bridge::testPattern()
{
	int totalEntity = kb->countEntity();
	int maxLeafPatternSize = 0;
	int sumLeafPatternSize = 0;
	int maxPatternSize = 0;
	int sumPatternSize = 0;

	for (int i = 1; i <= totalEntity; i ++)
	{
		unordered_set<int> anc;
		anc.clear();

		int totalBelong = kb->getBelongCount(i);
		for (int j = 0; j < totalBelong; j ++)
		{
			int cid = kb->getBelongConcept(i, j);
			while (kb->getPreCount(cid))
			{
				anc.insert(cid);
				cid = kb->getPreNode(cid, 0);
			}
		}
		int curPatternSize = 0;
		for (unordered_set<int>::iterator it = anc.begin(); it != anc.end(); it ++)
			curPatternSize ++;
		maxPatternSize = max(maxPatternSize, curPatternSize);
		sumPatternSize += curPatternSize;
		maxLeafPatternSize = max(maxLeafPatternSize, totalBelong);
		sumLeafPatternSize += totalBelong;
	}

	cout << "Maximum leaf size of a pattern: " << endl << "      "
		<< maxLeafPatternSize << endl;
	cout << "Average leaf size of a pattern: " << endl << "      "
		<< (int) ((double) sumLeafPatternSize / totalEntity) << endl;
	cout << "Maximum size of a pattern: " << endl << "      "
		<< maxPatternSize << endl;
	cout << "Average size of a pattern: " << endl << "      "
		<< (int) ((double) sumPatternSize / totalEntity) << endl;
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
				for (IterIT it1 = conSchema[cur].begin();
					it1 != conSchema[cur].end(); it1 ++)
				{
					cout << kb->getRelation(it1->first) << " : " << endl << "    ";
					IterII it2 = ((it1->second)->c).begin();
					for (int i = 1; i <= 20 && it2 != ((it1->second)->c).end(); i ++, it2 ++)
						cout << kb->getConcept(it2->first) << "    ";
					cout << endl;
					cout << endl;
				}
				cout << endl;
				break;
		}
		cout << "-------------------------------------" << endl << endl;
	}
}
