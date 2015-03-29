#include "Bridge.h"
#include <set>
#include <map>
#include <vector>
#include <string>
#include <utility>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
using namespace std;

Bridge::Bridge()
{
	cout << "Initializing Bridge!" << endl;

	//KB and Corpus
	kb = new YAGO();
	corpus = new WWT();

	//initialize match result
	initMatch();

	//initialize kb property
	initKbProperty();

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

void Bridge::initKbProperty()
{
	cout << "Initiliazing Bridge::kbProperty!" << endl;

	int totalConcept = kb->countConcept();
	int root = kb->getRoot();

	//initialize the unorderd_map array
	kbProperty.resize(totalConcept + 1);
	for (int i = 1; i <= totalConcept; i ++)
		kbProperty[i].clear();

	//make the KB property recursively
	cout << "Starting bridge::makeSchema dfs!" << endl;

	makeSchema(root);

/*
	int sum = 0;
	for (int i = 1; i <= totalConcept; i ++)
		for (unordered_map<int, TaxoPattern *>::iterator it = kbProperty[i].begin();
			it != kbProperty[i].end(); it ++)
			sum += ((it->second)->w).size();
	cout << "Total Kb property size: " << endl << "      " << sum << endl;
	cout << "Average Kb property size: " << endl << "      " << (double) sum / totalConcept << endl;
*/
}

void Bridge::makeSchema(int curNode)
{
	//instances of curNode
	int totalPossess = kb->getPossessCount(curNode);

	for (int i = 0; i < totalPossess; i ++)
	{
		int entityX = kb->getPossessEntity(curNode, i);
		int totalRelatedFact = kb->getFactCount(entityX);

		for (int j = 0; j < totalRelatedFact; j ++)
		{
			//entityX <curRelation> entityY
			pair<int, int> curPair = kb->getFactPair(entityX, j);
			int curRelation = curPair.first;
			int entityY = curPair.second;

			//use the type information of entityY
			int totalBelong = kb->getBelongCount(entityY);
			for (int k = 0; k < totalBelong; k ++)
			{
				int curConcept = kb->getBelongConcept(entityY, k);
				while (1)
				{
					if (! kbProperty[curNode].count(curRelation))
						kbProperty[curNode][curRelation] = new TaxoPattern();
					kbProperty[curNode][curRelation]->w[curConcept] ++;
					if (kb->getPreCount(curConcept) == 0)
						break;
					curConcept = kb->getPreNode(curConcept, 0);
				}
			}
		}
	}

	//Children
	int totalSuc = kb->getSucCount(curNode);
	for (int i = 0; i < totalSuc; i ++)
	{
		int curSuc = kb->getSucNode(curNode, i);
		makeSchema(curSuc);

		//aggregate
		unordered_map<int, TaxoPattern *> &curMap = kbProperty[curSuc];
		for (unordered_map<int, TaxoPattern *>::iterator it1 = curMap.begin();
			it1 != curMap.end(); it1 ++)
		{
			unordered_map<int, int> &curPatternMap = it1->second->w;
			for (unordered_map<int, int>::iterator it2 = curPatternMap.begin();
				it2 != curPatternMap.end(); it2 ++)
			{
				if (! kbProperty[curNode].count(it1->first))
					kbProperty[curNode][it1->first] = new TaxoPattern();

				kbProperty[curNode][it1->first]->w[it2->first] += it2->second;
			}
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

	//make patterns for lucky cells
	for (int i = 1; i <= totalCell; i ++)
	{
		if (matches[i].size() == 0)
			continue;

		cellPattern[i] = new TaxoPattern();
		for (int j = 0; j < matches[i].size(); j ++)
		{
			int curEntity = matches[i][j];
			int totalBelong = kb->getBelongCount(curEntity);
			for (int k = 0; k < totalBelong; k ++)
			{
				int curConcept = kb->getBelongConcept(curEntity, k);
				while (1)
				{
					cellPattern[i]->w[curConcept] ++;
					if (kb->getPreCount(curConcept) == 0)
						break;
					curConcept = kb->getPreNode(curConcept, 0);
				}
			}
		}
	}
	
	//make patterns for unlucky cells
	for (int i = 1; i <= totalTable; i ++)
	{
		int nRow = allTables[i].nRow;
		int nCol = allTables[i].nCol;

		for (int y = 0; y < nCol; y ++)
		{
			TaxoPattern *colPattern = new TaxoPattern();
			colPattern->w[kb->getRoot()] = 1;

			for (int x = 0; x < nRow; x ++)
			{
				Cell cur = allTables[i].cells[x][y];
				if (matches[cur.id].size() == 0)
					continue;
				//merge
				unordered_map<int, int> curMap = cellPattern[cur.id]->w;
				for (unordered_map<int, int>::iterator it = curMap.begin(); 
					it != curMap.end(); it ++)
					colPattern->w[it->first] += it->second;
			}

			for (int x = 0; x < nRow; x ++)
			{
				Cell cur = allTables[i].cells[x][y];
				if (matches[cur.id].size())
					continue;
				cellPattern[cur.id] = colPattern;
			}
		}
	}
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
					<< (kbProperty[cur]).size() << endl;
				break;
			case 8 :
				cout << "Sample Relations are: " << endl << endl;
				for (unordered_map<int, TaxoPattern *>::iterator it1 = kbProperty[cur].begin();
					it1 != kbProperty[cur].end(); it1 ++)
				{
					cout << kb->getRelation(it1->first) << " : " << endl << "    ";
					unordered_map<int, int>::iterator it2 = ((it1->second)->w).begin();
					for (int i = 1; i <= 5 && it2 != ((it1->second)->w).end(); i ++, it2 ++)
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
