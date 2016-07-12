#include "Experiment.h"
#include <cctype>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sys/time.h>
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
	string wrongFileName = "../../../data/Result/colConcept/colConcept_Majority_wrong.txt";
	ofstream wrongFile(wrongFileName.c_str());
	int K = 3;
	srand(time(0));

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
	for (int i = 0; i < (int) tids.size(); i ++)
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
			else
			{
				wrongFile << endl << "Wrong sample: " << endl
				     << "table_id = " << tid
				     << "  column_id = " << cid << endl;
				wrongFile << "GT answers: " << endl << '\t';
				for (auto o : gts[i])
					wrongFile << o << '\t';
				wrongFile << endl;
				wrongFile << "Naive answers: " << endl << '\t';
				for (auto o : ans)
					wrongFile << bridge->kb->getConcept(o) << '\t';
				wrongFile << endl << endl;
			}
		}
	}
	resultFile.close();
	wrongFile.close();
	cout << "Number of queries: " << numQuery << endl <<
			"correct queries: " << acQuery << endl <<
			"Accuracy: " << (double) acQuery / numQuery << endl;
}

void Experiment::runColRelationLatency()
{
	//Test findRelation running time
	int nTable = bridge->corpus->countTable();
	double totalTime = 0;

	srand(time(0));
	for (int lp = 1; lp <= 1000; lp ++)
	{
		int curTable = rand() % nTable + 1;
		int col = -1;
		for (int t = 0; t < 50; t ++)
		{
			int nCol = bridge->corpus->getTable(curTable).nCol;
			int entityCol = bridge->corpus->getTable(curTable).entityCol;
			if (col >= 0 && col < nCol && col != entityCol)
				break;
			col = rand() % nCol;
		}

		struct timeval t1, t2;
		gettimeofday(&t1, NULL);
		bridge->findRelation(bridge->corpus->getTable(curTable).table_id, col, true);
		gettimeofday(&t2, NULL);

		totalTime += t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) / 1000000.0;
		if (lp % 50 == 0)
			cout << "breakpoint : " << lp / 50 << endl;
	}
	cout << "Average findRelation latency: " << totalTime / 1000.0 << endl;
}

void Experiment::runRecConceptLatency()
{
	//Test findConcept running time
	int nTable = bridge->corpus->countTable();
	double totalTime = 0;

	srand(time(0));
	for (int lp = 1; lp <= 1000; lp ++)
	{
		int curTable = rand() % nTable + 1;
		int row = rand() % bridge->corpus->getTable(curTable).nRow;
		cout << curTable << " " << row << endl;
		struct timeval t1, t2;
		gettimeofday(&t1, NULL);
		bridge->findRecordConcept(bridge->corpus->getTable(curTable).table_id, row, true);
		gettimeofday(&t2, NULL);

		totalTime += t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) / 1000000.0;
	}
	cout << "Average findConcept latency: " << totalTime / 1000.0 << endl;

	//Output the maximum running time
	vector<int> nCols;
	for (int i = 0; i < nTable; i ++)
		nCols.push_back(bridge->corpus->getTable(i + 1).nCol);
	sort(nCols.begin(), nCols.end());
	for (int i = 0; i < (int) nCols.size(); i ++)
		cout << nCols[i] << endl;
}

bool Experiment::isEqualConcept(string c1, string c2)
{
	int posLowLine = -1;
	for (int i = 0; i < (int) c1.size(); i ++)
		if (c1[i] == '_')
			posLowLine = i;
	if (posLowLine != -1)
		c1 = c1.substr(0, posLowLine);

	posLowLine = -1;
	for (int i = 0; i < (int) c2.size(); i ++)
		if (c2[i] == '_')
			posLowLine = i;
	if (posLowLine != -1)
		c2 = c2.substr(0, posLowLine);
	return c1 == c2;
}
