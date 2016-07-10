#include "Experiment.h"
#include <cctype>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
using namespace std;

Experiment::Experiment(Bridge *b) : bridge(b) {}

void Experiment::runAllExp()
{
	runExpColConceptNaive();
}

void Experiment::runExpColConceptNaive()
{
	cout << endl
		 << "Current Experiment: running the naive majority method of column concept determination..."
		 << endl;

	//some variables
	vector<int> tids, cids, nGTs;
	vector<vector<string>> gts;
	string gtFileName = "../../../data/GT/Column_Concept_GT.txt";
	ifstream gtFile(gtFileName.c_str());
	string resultFileName = "../../../data/Result/colConcept/colConcept_Majority.txt";
	ofstream resultFile(resultFileName.c_str());
	int K = 3;

	//read in the gt file
	int tid, cid, nGT;
	string gt;
	while (gtFile >> tid >> cid >> nGT)
	{
		tids.push_back(tid);
		cids.push_back(cid);
		nGTs.push_back(nGT);
		gts.push_back(vector<string>());

		for (int i = 0; i < nGT; i ++)
		{
			gtFile >> gt;
			gts[gts.size() - 1].push_back(gt);
		}
	}
	gtFile.close();

	//run functions in ColConcept.cpp
	int numQuery = 0;
	int acQuery = 0;
	for (int i = 0; i < tids.size(); i ++)
	{
		int tid = tids[i];
		int cid = cids[i];
		vector<int> ans = bridge->findColConceptMajority(tid, cid, false);

		//output to file
		resultFile << tid << " " << cid << " " << min((int) ans.size(), K) << " ";
		for (int j = 0; j < min((int) ans.size(), K); j ++)
			resultFile << bridge->kb->getConcept(ans[j]) << " ";
		resultFile << endl;

		//update accuracy
		if (nGTs[i] == 1)
		{
			numQuery ++;
			bool ac = false;
			for (int j = 0; j < min((int) ans.size(), K); j ++)
				if (isEqualConcept(bridge->kb->getConcept(ans[j]), gts[i][0]))
					ac = true;
			if (ac)
				acQuery ++;
		}
	}
	cout << "Number of queries: " << numQuery << endl <<
			"correct queries: " << acQuery << endl <<
			"Accuracy: " << (double) acQuery / numQuery << endl;
}

bool Experiment::isEqualConcept(string c1, string c2)
{
	int posLowLine = -1;
	for (int i = 0; i < c1.size(); i ++)
		if (c1[i] == '_')
			posLowLine = i;
	if (posLowLine != -1)
		c1 = c1.substr(0, posLowLine);

	posLowLine = -1;
	for (int i = 0; i < c2.size(); i ++)
		if (c2[i] == '_')
			posLowLine = i;
	if (posLowLine != -1)
		c2 = c2.substr(0, posLowLine);
	return c1 == c2;
}
