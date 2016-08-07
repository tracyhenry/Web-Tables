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
	vector<string> col_methods = {"baseline", "katara", "ours"};
	vector<string> rec_methods = {"ours"};
	//column concepts experiments
	for (string method : col_methods)
		runExpColConcept(method);

	//column relationship experiments
	for (string method : col_methods)
		runExpColRelation(method);

	//record concepts experiments
	for (string method : rec_methods)
		runExpRecConcept(method);
}

vector<double> Experiment::runExpColConcept(string method)
{
	//some variables
	vector<int> tids, cids, nGTs;
	vector<vector<string>> gts;
	string gtFileName = "../../../data/GT/Column_Concept_GT.txt";
	ifstream gtFile(gtFileName.c_str());
	string resultFileName = "../../../data/Result/colConcept/colConcept_" + method + ".txt";
	ofstream resultFile(resultFileName.c_str());
	string wrongFileName = "../../../data/Result/colConcept/colConcept_" + method + "_wrong.txt";
	ofstream wrongFile(wrongFileName.c_str());
	srand(time(0));
	cout << "The " << method << " method of column concept determination is running......" << endl;

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
	unordered_map<int, vector<int>> outputAns;
	for (int i = 0; i < (int) tids.size(); i ++)
		if (! outputAns.count(tids[i]))
		{
			if (method == "baseline")
				outputAns[tids[i]] = bridge->baselineFindColConceptAndRelation(tids[i], false);
			else if (method == "katara")
				outputAns[tids[i]] = bridge->kataraFindColConceptAndRelation(tids[i], false);
			else if (method == "ours")
				outputAns[tids[i]] = bridge->findColConceptAndRelation(tids[i], false);
		}

	double acQuery = 0;
	double numOutput = 0;
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
		double score = 0;
		for (string concept : gts[i])
			score = max(score, getScore(concept, label));
		acQuery += score;
		if (score < 1.0)
		{
			wrongFile << endl << "Wrong sample: " << endl
						<< "table_id = " << tid << " "
						<< "column_id = " << cid << " "
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

	return calculatePRF(acQuery, numOutput, (double) tids.size(), true);
}

vector<double> Experiment::runExpRecConcept(string method)
{
	//some variables
	vector<int> tids, rids, nGTs;
	vector<unordered_map<int, double>> gts;
	string gtFileName = "../../../data/GT/Record_Concept_GT.txt";
	ifstream gtFile(gtFileName.c_str());
	string resultFileName = "../../../data/Result/recConcept/recConcept_" + method + ".txt";
	ofstream resultFile(resultFileName.c_str());
	srand(time(0));
	cout << "The " << method << " method of record concept determination is running......" << endl;

	//read in the gt file
	int tid, rid, nGT;
	string gt;
	double gtScore;
	while (gtFile >> tid >> rid >> nGT)
	{
		tids.push_back(tid);
		rids.push_back(rid);
		nGTs.push_back(nGT);
		gts.push_back(unordered_map<int, double>());
		unordered_map<int, double> &curGT = gts[(int) gts.size() - 1];
		for (int i = 0; i < nGT; i ++)
		{
			gtFile >> gt >> gtScore;
			int gtId = bridge->kb->getConceptId(gt);
			curGT[gtId] = gtScore;
		}
	}
	gtFile.close();

	int arrayKs[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	vector<int> Ks(arrayKs, arrayKs + sizeof(arrayKs) / sizeof(int));
	vector<double> avgPrecision(Ks.size());
	vector<double> avgRecall(Ks.size());
	vector<double> avgNDCG(Ks.size());

	//run functions in RecConcept.cpp
	vector<vector<int>> output(tids.size());
	for (int i = 0; i < (int) tids.size(); i ++)
		if (method == "ours")
			output[i] = bridge->findRecordConcept(tids[i], rids[i], Ks.back(), false);

	for (int i = 0; i < (int) tids.size(); i ++)
	{
		int tid = tids[i];
		int rid = rids[i];
		for (int j = 0; j < (int) Ks.size(); j ++)
		{
			int K = Ks[j];
			double ac = 0;
			for (int k = 0; k < K; k ++)
				if (gts[i].count(output[i][k]))
					ac ++;
			//prf
			vector<double> prf = calculatePRF(ac, K, gts[i].size(), false);
			if (K <= (int) gts[i].size())
				avgPrecision[j] += prf[0];
			if (K == (int) gts[i].size())
				avgRecall[j] += prf[1];

			//dcg
			double dcg = (gts[i].count(output[i][0]) ? gts[i][output[i][0]] : 0);
			for (int k = 1; k < K; k ++)
			{
				double curScore = 0;
				if (gts[i].count(output[i][k]))
					curScore = gts[i][output[i][k]];
				curScore /= log2(k + 1);
				dcg += curScore;
			}
			//idcg
			vector<int> gtScores;
			for (auto kv : gts[i])
				gtScores.push_back(- kv.second);
			sort(gtScores.begin(), gtScores.end());
			double idcg = - gtScores[0];
			for (int k = 1; k < K; k ++)
			{
				double curScore = 0;
				if (k < (int) gtScores.size())
					curScore = - gtScores[k];
				curScore /= log2(k + 1);
				idcg += curScore;
			}
			//ndcg
			double ndcg = dcg / idcg;
			avgNDCG[j] += ndcg;
		}
		//output to result file
		resultFile << "Table_id = " << tid << " " << "Row_id = " << rid << endl << endl;
		for (int c : output[i])
			resultFile << '\t' << bridge->kb->getConcept(c) << '\t' <<
				(gts[i].count(c) ? gts[i][c] : 0) << endl;
		resultFile << endl;
	}
	resultFile.close();
	//output
	for (int i = 0; i < (int) Ks.size(); i ++)
	{
		int K = Ks[i];
		int total = 0;
		for (int j = 0; j < (int) gts.size(); j ++)
			if (K <= (int) gts[j].size())
				total ++;
		avgPrecision[i] /= (double) total;
		avgNDCG[i] /= (double) (int) tids.size();
	}
	cout << "Ks :" << endl;
	for (int i = 0; i < (int) Ks.size(); i ++)
		cout << '\t' << Ks[i];
	cout << endl;
	cout << "Presision:  " << endl;
	for (int i = 0; i < (int) Ks.size(); i ++)
		printf("\t%.2f%%", avgPrecision[i] * 100.0);
	cout << endl;
	cout << "NDCG: " << endl;
	for (int i = 0; i < (int) Ks.size(); i ++)
		printf("\t%.2f%%", avgNDCG[i] * 100.0);
	cout << endl;
	cout << "Recall: " << endl;
	double harmonyRecall = 0;
	for (int i = 0; i < (int) Ks.size(); i ++)
		harmonyRecall += avgRecall[i];
	harmonyRecall /= (double) gts.size();
	printf("\t%.2f%%", harmonyRecall * 100.0);

	//return
	vector<double> ans;
	for (int p : avgPrecision)
		ans.push_back(p);
	for (int r : avgRecall)
		ans.push_back(r);
	for (int ndcg : avgNDCG)
		ans.push_back(ndcg);
	return ans;
}

vector<double> Experiment::runExpColRelation(string method)
{
	//some variables
	vector<int> tids, cids, nGTs;
	vector<vector<string>> gts;
	string gtFileName = "../../../data/GT/Relationship_GT.txt";
	ifstream gtFile(gtFileName.c_str());
	string resultFileName = "../../../data/Result/colRelation/colRelation_" + method + ".txt";
	ofstream resultFile(resultFileName.c_str());
	string wrongFileName = "../../../data/Result/colRelation/colRelation_" + method + "_wrong.txt";
	ofstream wrongFile(wrongFileName.c_str());
	srand(time(0));
	cout << "The " << method << " method of binary relationship determination is running......" << endl;

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
			for (int it = 0; it < (int) gt.size(); it ++)
				if (gt[it] == '_')
					gt[it] = ' ';
			gts[gts.size() - 1].push_back(gt);
		}
	}
	gtFile.close();

	//run functions in ColConcept.cpp
	unordered_map<int, vector<int>> outputAns;
	for (int i = 0; i < (int) tids.size(); i ++)
		if (! outputAns.count(tids[i]))
		{
			if (method == "baseline")
				outputAns[tids[i]] = bridge->baselineFindColConceptAndRelation(tids[i], false);
			else if (method == "katara")
				outputAns[tids[i]] = bridge->kataraFindColConceptAndRelation(tids[i], false);
			else if (method == "ours")
				outputAns[tids[i]] = bridge->findColConceptAndRelation(tids[i], false);
		}

	double acQuery = 0;
	double numOutput = 0;
	for (int i = 0; i < (int) tids.size(); i ++)
	{
		int tid = tids[i];
		int cid = cids[i];
		int labelId = outputAns[tid][cid + (int) outputAns[tid].size() / 2];
		string label = "NULL";
		if (labelId != -1)
			label = bridge->kb->getRelation(labelId), numOutput ++;

		//output to file
		resultFile << tid << '\t' << cid << '\t' << 1 << '\t'
					<< label << '\t' << endl;

		//update metrics
		double score = 0;
		for (string rel : gts[i])
			if (rel == label)
				score = 1.0;
		acQuery += score;
		if (score < 1.0)
		{
			wrongFile << endl << "Wrong sample: " << endl
						<< "table_id = " << tid << " "
						<< "column_id = " << cid << " "
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

	return calculatePRF(acQuery, numOutput, (double) tids.size(), true);
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
		bridge->findRecordConcept(bridge->corpus->getTable(curTable).table_id, row, 10, true);
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
	if (! bridge->kb->isDescendant(conceptId, labelId))
		return 0;
	//else return exp(-x), where x is the steps between them
	int conceptLevel = bridge->kb->getLevel(conceptId);
	int labelLevel = bridge->kb->getLevel(labelId);
	return exp(-(double)abs(conceptLevel - labelLevel));
}

vector<double> Experiment::calculatePRF(double ac, double numOut, double total, bool print)
{
	double precision = (double) ac / numOut;
	double recall = (double) ac / total;
	double fvalue;
	if (fabs(precision + recall) > 1e-9)
		fvalue = 2.0 * precision * recall  / (precision + recall);
	else
		fvalue = 0;

	if (print)
	{
		cout << ac << " " << numOut << " " << total << endl;
		printf("Precision : %.2f%%\n", precision * 100.0);
		printf("Recall : %.2f%%\n", recall * 100.0);
		printf("F-Value %.2f%%\n", fvalue * 100.0);
	}
	vector<double> ans;
	ans.push_back(precision);
	ans.push_back(recall);
	ans.push_back(fvalue);

	return ans;
}
