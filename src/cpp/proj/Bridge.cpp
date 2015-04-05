#include "Bridge.h"
#include "Matcher.h"
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

	//debug file
	debug.open("debug.txt");

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

			//new taxo pattern & add the entity
			if (! kbProperty[curNode].count(curRelation))
				kbProperty[curNode][curRelation] = new TaxoPattern();
			kbProperty[curNode][curRelation]->e[entityY] ++;

			//use the type information of entityY
			int totalBelong = kb->getBelongCount(entityY);
			for (int k = 0; k < totalBelong; k ++)
			{
				int curConcept = kb->getBelongConcept(entityY, k);
				while (1)
				{
					kbProperty[curNode][curRelation]->c[curConcept] ++;
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

		//loop over all relations
		unordered_map<int, TaxoPattern *> &curMap = kbProperty[curSuc];
		for (unordered_map<int, TaxoPattern *>::iterator it1 = curMap.begin();
			it1 != curMap.end(); it1 ++)
		{
			//new taxo pattern
			if (! kbProperty[curNode].count(it1->first))
				kbProperty[curNode][it1->first] = new TaxoPattern();

			//merge concepts
			unordered_map<int, int> &curConceptMap = it1->second->c;
			for (unordered_map<int, int>::iterator it2 = curConceptMap.begin();
				it2 != curConceptMap.end(); it2 ++)
				kbProperty[curNode][it1->first]->c[it2->first] += it2->second;

			//merge entities
			unordered_map<int, int> &curEntityMap = it1->second->e;
			for (unordered_map<int, int>::iterator it2 = curEntityMap.begin();
				it2 != curEntityMap.end(); it2 ++)
				kbProperty[curNode][it1->first]->e[it2->first] += it2->second;
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
			cellPattern[i]->e[curEntity] ++;

			int totalBelong = kb->getBelongCount(curEntity);
			for (int k = 0; k < totalBelong; k ++)
			{
				int curConcept = kb->getBelongConcept(curEntity, k);
				while (1)
				{
					cellPattern[i]->c[curConcept] ++;
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
		Table curTable = corpus->getTable(i);
		int nRow = curTable.nRow;
		int nCol = curTable.nCol;

		for (int y = 0; y < nCol; y ++)
		{
			TaxoPattern *colPattern = new TaxoPattern();
			colPattern->c[kb->getRoot()] = 1;

			for (int x = 0; x < nRow; x ++)
			{
				Cell cur = curTable.cells[x][y];
				if (matches[cur.id].size() == 0)
					continue;

				//merge concepts
				unordered_map<int, int> &curConceptMap = cellPattern[cur.id]->c;
				for (unordered_map<int, int>::iterator it = curConceptMap.begin();
					it != curConceptMap.end(); it ++)
					colPattern->c[it->first] += it->second;

				//merge entities
				unordered_map<int, int> &curEntityMap = cellPattern[cur.id]->e;
				for (unordered_map<int, int>::iterator it = curEntityMap.begin();
					it != curEntityMap.end(); it ++)
					colPattern->e[it->first] += it->second;
			}

			for (int x = 0; x < nRow; x ++)
			{
				Cell cur = curTable.cells[x][y];
				if (matches[cur.id].size())
					continue;
				cellPattern[cur.id] = colPattern;
			}
		}
	}

/*	//stats
	int sum = 0;
	for (int i = 1; i <= totalCell; i ++)
		sum += cellPattern[i]->w.size();

	cout << "Average Pattern Size : " << endl << "      " << double(sum) / totalCell << endl;*/
}

TaxoPattern *Bridge::getKbProperty(int conceptId, int relationId, bool isDebug)
{
	TaxoPattern *ans = kbProperty[conceptId][relationId];
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
	int testCid = 213487;
	unordered_map<int, int> &C = p->c;

	//concepts
	vector<pair<int, int>> tmp; tmp.clear();
	int sum = 1e-9;
	for (unordered_map<int, int>::iterator it = C.begin(); it != C.end(); it ++)
		if (cellPattern[testCid]->c.count(it->first))
			tmp.emplace_back(- it->second, it->first);

	sort(tmp.begin(), tmp.end());
	for (int i = 0; i < tmp.size(); i ++)
		debug << tmp[i].second << " " << kb->getConcept(tmp[i].second)
			<< " : " << - tmp[i].first
			<< " " << kb->getDepth(tmp[i].second) << endl;

	//entities
	unordered_map<int, int> &E = p->e;
	for (unordered_map<int, int>::iterator it = E.begin(); it != E.end(); it ++)
		if (cellPattern[testCid]->e.count(it->first))
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
					<< (kbProperty[cur]).size() << endl;
				break;
			case 8 :
				cout << "Sample Relations are: " << endl << endl;
				for (unordered_map<int, TaxoPattern *>::iterator it1 = kbProperty[cur].begin();
					it1 != kbProperty[cur].end(); it1 ++)
				{
					cout << kb->getRelation(it1->first) << " : " << endl << "    ";
					unordered_map<int, int>::iterator it2 = ((it1->second)->c).begin();
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

void Bridge::tableQuery()
{
	while (1)
	{
		int tid, r, c, cid;
		cin >> tid;
		if (tid == -1)
			break;
		cin >> r >> c;
		cid = corpus->getTableByDataId(tid).cells[r][c].id;

		unordered_map<int, int> &tmp = cellPattern[cid]->c;
		cout << endl << "-----------------------------------------------------" << endl;
		for (unordered_map<int, int>::iterator it = tmp.begin(); it != tmp.end(); it ++)
			cout << it->first << " " << kb->getConcept(it->first) << ": " << it->second << endl;
		cout << endl << "-----------------------------------------------------" << endl;
	}
}

void Bridge::findConcept(int tid, int r)
{
	//Brute force
	int totalConcept = kb->countConcept();

	//Similarity Array
	vector<pair<double, int>> simScore;
	simScore.clear();
	simScore.resize(totalConcept + 1);

	//information about the current record
	Table curTable = corpus->getTableByDataId(tid);
	int nCol = curTable.nCol;
	int entityCol = curTable.entityCol;
	int cid = curTable.cells[r][entityCol].id;

	//debug
	getKbProperty(70366, kb->getRelationId("created"), true);
	getKbProperty(114102, kb->getRelationId("created"), true);
	getCellPattern(curTable.cells[14][2].id, true);
	for (int i = 1; i <= kb->countRelation(); i ++)
		getKbProperty(114279, i, true);

	//loop over all concepts
	for (int i = 1; i <= totalConcept; i ++)
	{
//		if (! cellPattern[cid]->w.count(i))
//			continue;
//		if (kb->getSucCount(i)) continue;
		double sumSim = 0;

		//loop over all attributes
		for (int c = 0; c < nCol; c ++)
		{
			if (c == entityCol) continue;
			double sim = 0;

			//loop over all properties
			for (unordered_map<int, TaxoPattern *>::iterator it1 = kbProperty[i].begin();
				it1 != kbProperty[i].end(); it1 ++)
			{
				TaxoPattern *cp = cellPattern[curTable.cells[r][c].id];
				TaxoPattern *pp = it1->second;
				sim = max(sim, Matcher::expoDepth(kb, cp, pp));
			}
			sumSim += sim;
		}
		simScore.emplace_back(- sumSim, i);
	}

	//sort
	sort(simScore.begin(), simScore.end());

	//output
	cout << endl << "Top 50 Answers: " << endl;
	for (int i = 0; i < min((int) simScore.size(), 50); i ++)
		if (- simScore[i].first > 0)
			cout << - simScore[i].first << " " << simScore[i].second
				<< " " << kb->getConcept(simScore[i].second)
				<< " " << kb->getDepth(simScore[i].second) << endl;
}
