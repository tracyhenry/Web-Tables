#include "KB.h"
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <utility>
#include <iostream>
#include <algorithm>
using namespace std;


KB::KB() :
	dirPath("/home/wenbo/Web-Tables/data/KB/"), delim("/") 
{}

int KB::countConcept()
{
	return N;
}

int KB::countEntity()
{
	return K;
}

int KB::getPreCount(int conceptId)
{
	return pre[conceptId].size();
}

int KB::getPreNode(int conceptId, int index)
{
	return pre[conceptId][index];
}

int KB::getSucCount(int conceptId)
{
	return suc[conceptId].size();
}

int KB::getSucNode(int conceptId, int index)
{
	return suc[conceptId][index];
}

int KB::getBelongCount(int entityId)
{
	return belongs[entityId].size();
}

int KB::getBelongConcept(int entityId, int index)
{
	return belongs[entityId][index];
}

YAGO::YAGO()
{
	conceptFileName = dirPath + "Concepts.txt";
	subclassFileName = dirPath + "SubClass.txt";
	entityFileName = dirPath + "Entities.txt";
	typeFileName = dirPath + "Types.txt";
	relationFileName = dirPath + "Relations.txt";
	factFileName = dirPath + "Facts.txt";

	cout << "Initializing YAGO!" << endl;

	initTaxonomy();
	initType();
	initFact();
}

void YAGO::initTaxonomy()
{

	//YAGO's Input File
	ifstream conceptFile(conceptFileName.c_str());

	FILE *subclassFile = fopen(subclassFileName.c_str(), "r");

	//make the maps
	M.clear();
	MM.clear();
	N = 0;

	string c;
	while (getline(conceptFile, c))
		M[c] = ++ N, MM[N] = c;
 	conceptFile.close();

	//make graph
	pre.clear(), suc.clear();
	pre.resize(N + 1), suc.resize(N + 1);
	for (int i = 1; i <= N; i ++)
		pre[i].clear(), suc[i].clear();

	int x, y;
	while (fscanf(subclassFile, "%d%d", &x, &y) == 2)
		pre[x].push_back(y), suc[y].push_back(x);
}

void YAGO::initType()
{
	ifstream entityFile(entityFileName.c_str());

	FILE *typeFile = fopen(typeFileName.c_str(), "r");

	//Get entity strings
	E.clear();
	EE.clear();
	K = 0;
	string e;
	while (getline(entityFile, e))
		E[e] = ++ K, EE[K] = e;

	//make relationship
	int x, y;
	possess.resize(N + 1);
	belongs.resize(K + 1);
	for (int i = 1; i <= N; i ++)
		possess[i].clear();
	for (int i = 1; i <= K; i ++)
		belongs[i].clear();
	while (fscanf(typeFile, "%d%d", &x, &y) == 2)
		belongs[x].push_back(y), possess[y].push_back(x);
}

void YAGO::initFact()
{
	ifstream relationFile(relationFileName.c_str());

	FILE *factFile = fopen(factFileName.c_str(), "r");

	//Get relation names
	R.clear();
	RR.clear();
	F = 0;
	string r;
	while (getline(relationFile, r))
		R[r] = ++ F, RR[F] = r;

	//Get facts
	int x, z, y;
	facts.resize(K + 1);
	for (int i = 1; i <= K; i ++)
		facts[i].clear();
	while (fscanf(factFile, "%d%d%d", &x, &z, &y) == 3)
		facts[x].emplace_back(z, y);
}

void YAGO::getConceptWithMostFacts()
{
	vector<pair<int, string>> mmm;
	mmm.clear();
	for (int i = 1; i <= N; i ++)
	{
		if (MM[i].substr(0, 4) != "word")
			continue;
		int tot = 0;
		for (int j = 0; j < possess[i].size(); j ++)
		{
			int x = possess[i][j];
			tot += facts[x].size();
		}
		mmm.emplace_back(-tot, MM[i]);
	}
	sort(mmm.begin(), mmm.end());
	for (int i = 0; i < 20; i ++)
		cout << mmm[i].second << " : " << -mmm[i].first << endl;
}

void YAGO::traverse()
{
	int cur = M["owl:Thing"];
	vector<int> q; q.clear();
	q.push_back(cur);

	while (1)
	{
		cur = q[q.size() - 1];
		cout << "We are at : " << MM[cur] << endl << "Input your operation: " << endl;

		int x, tmp;
		string s;
		cin >> x;

		switch (x)
		{
			case 1 : 
				//print out the number of successors
				cout << "The number of successors: " << endl << suc[cur].size() << endl;
				break;
			case 2 : 
				//print all the successors
				cout << "The successors are: " << endl;
				for (int i = 0; i < suc[cur].size(); i ++)
					cout << MM[suc[cur][i]] << "     ";
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
				q.push_back(M[s]);
				break;
			case 5 :
				cout << possess[cur].size() << " entities in this concept! Sample entities are: " << endl;
				for (int i = 0; i < min((int) possess[cur].size(), 10); i ++)
					cout << "  " << EE[possess[cur][i]] << endl;
				break;
			case 6 :
				tmp = 0;
				for (int i = 0; i < possess[cur].size(); i ++)
				{
					int j = possess[cur][i];
					tmp += facts[j].size();
				}
				cout << "Number of facts related to this concept: " << endl << tmp << endl;
		}
		cout << "-------------------------------------" << endl << endl;
	}
}
