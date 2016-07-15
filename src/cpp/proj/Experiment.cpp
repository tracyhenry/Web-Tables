#include "Experiment.h"
#include <cmath>
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
//	runExpColConceptNaive();
	runExpColConceptGood();
}

vector<double> Experiment::runExpColConceptGood()
{
	cout << endl
		 << "Current Experiment: running the column concept & rel algorithm to get column concept labels."
		 << endl;

	//some variables
	vector<int> tids, cids, nGTs;
	vector<vector<string>> gts;
	string gtFileName = "../../../data/GT/Column_Concept_GT.txt";
	ifstream gtFile(gtFileName.c_str());
	string resultFileName = "../../../data/Result/colConcept/colConcept_Good.txt";
	ofstream resultFile(resultFileName.c_str());
	string wrongFileName = "../../../data/Result/colConcept/colConcept_Good_wrong.txt";
	ofstream wrongFile(wrongFileName.c_str());
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
	int acQuery = 0;
	int numOutput = 0;
	unordered_map<int, vector<int>> outputAns;
	for (int i = 0; i < (int) tids.size(); i ++)
		if (! outputAns.count(tids[i]))
			outputAns[tids[i]] = bridge->findColConceptAndRelation(tids[i], false);

	for (int i = 0; i < (int) tids.size(); i ++)
	{
		int tid = tids[i];
		int cid = cids[i];
		int labelId = outputAns[tid][cid];
		string label = "NULL";
		if (labelId != -1)
			label = bridge->kb->getConcept(labelId), numOutput ++;

		//output to file
		resultFile << tid << '\t' << cid << '\t' << 1 << '\t'
					<< label << '\t' << endl;

		//update metrics
		double score = -1;
		for (string concept : gts[i])
			score = max(score, getScore(concept, label));
		if (score == 1.0)
			acQuery ++;
		else
		{
			wrongFile << endl << "Wrong sample: " << endl
						<< "table_id = " << tid
						<< "column_id = " << cid
						<< "score = " << score << endl;
			wrongFile << "GT answers: " << endl << '\t';
			for (auto o : gts[i])
				wrongFile << o << '\t';
			wrongFile << endl;
			wrongFile << "Algo answers: " << endl << '\t';
			wrongFile << label << endl << endl;
		}
	}
	resultFile.close();
	wrongFile.close();

	return calculateMetrics(acQuery, numOutput, (int) tids.size());
}

vector<double> Experiment::runExpColConceptNaive()
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
	int K = 1;
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
	int numOutput = 0;
	int acQuery = 0;
	for (int i = 0; i < (int) tids.size(); i ++)
	{
		int tid = tids[i];
		int cid = cids[i];
		vector<int> ans = bridge->findColConceptMajority(tid, cid, false);
		if (ans[0] != -1)
			numOutput ++;

		//output to file
		resultFile << tid << " " << cid << " " << min((int) ans.size(), K) << " ";
		for (int j = 0; j < min((int) ans.size(), K); j ++)
			resultFile << bridge->kb->getConcept(ans[j]) << " ";
		resultFile << endl;

		//update accuracy
		double score = -1;
		for (string concept : gts[i])
			score = max(score, getScore(concept, bridge->kb->getConcept(ans[0])));
		if (score == 1.0)
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
			wrongFile << "score : " << score << endl << endl;
		}
	}
	resultFile.close();
	wrongFile.close();

	return calculateMetrics(acQuery, numOutput, (int) tids.size());
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

double Experiment::getScore(string concept, string label)
{
	//if not labeled, return -1
	if (label == "NULL")
		return -1;
	//if they are equal, return 1
	if (isEqualConcept(concept, label))
		return 1;
	//get concept ids
	int conceptId = bridge->kb->getConceptId(concept);
	int labelId = bridge->kb->getConceptId(label);
	//if one of them is not a concept in the kb, return -1
	if (conceptId == 0 || labelId == 0)
		return -1;
	//if neither of them is the other's ancestor, return 0
	if (! bridge->kb->isDescendant(conceptId, labelId) &&
		! bridge->kb->isDescendant(labelId, conceptId))
		return 0;
	//else return exp(-x), where x is the steps between them
	int conceptLevel = bridge->kb->getLevel(conceptId);
	int labelLevel = bridge->kb->getLevel(labelId);
	return exp(-(double)abs(conceptLevel - labelLevel));
}

vector<double> Experiment::calculateMetrics(int ac, int numOut, int total)
{
	double precision = (double) ac / numOut;
	double recall = (double) ac / (double) total;
	double fvalue = 2.0 * precision * recall  / (precision + recall);

	cout << ac << " " << numOut << " " << total << endl;
	printf("Precision : %.2f%%\n", precision * 100.0);
	printf("Recall : %.2f%%\n", recall * 100.0);
	printf("F-Value %.2f%%\n", fvalue * 100.0);

	vector<double> ans;
	ans.push_back(precision);
	ans.push_back(recall);
	ans.push_back(fvalue);

	return ans;
}
