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

}

void Bridge::initMatch()
{
	cout << "Initializing match!!!!" << endl;
	//Match FileName
	string matchFileName = "../../../data/fuzzy/result_jaccard_0.8_0.8.txt";
	ifstream matchFile(matchFileName);

	string s;
	matches.clear();
	while (getline(matchFile, s))
	{
		//line 1
		stringstream sin(s);
		double sim;
		int entityId, cellId;

		sin >> sim >> entityId >> cellId;
		entityId ++;
		cellId ++;
		matches.emplace_back(entityId, cellId);

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

	makeSchema(kb->getRoot());

	int rt = kb->getRoot();
	int sum = 0;
	for (unordered_map<int, TaxoPattern *>::iterator it = kbProperty[rt].begin(); it != kbProperty[rt].end(); it ++)
		sum += ((it->second)->w).size();
	cout << sum << endl;
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
		for (unordered_map<int, TaxoPattern *>::iterator it1 = curMap.begin(); it1 != curMap.end(); it1 ++)
		{
			unordered_map<int, int> &curPatternMap = it1->second->w;
			for (unordered_map<int, int>::iterator it2 = curPatternMap.begin(); it2 != curPatternMap.end(); it2 ++)
			{
				if (! kbProperty[curNode].count(it1->first))
					kbProperty[curNode][it1->first] = new TaxoPattern();
				kbProperty[curNode][it1->first]->w[it2->first] += it2->second;
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
	kb->traverse();
}

