#include "KB.h"
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <utility>
#include <iostream>
using namespace std;


KB::KB() : 
	dirPath("/home/wenbo/Web-Tables/data/KB"), delim("/") 
{}


YAGO::YAGO()
{
	conceptFileName = dirPath + delim + "Concepts.txt";
	subclassFileName = dirPath + delim + "SubClass.txt";
	entityFileName = dirPath + delim + "Entities.txt";
	typeFileName = dirPath + delim + "Types.txt";
	factFileName = dirPath + delim + "Facts.txt";

	InitTaxonomy();
	InitType();
	InitFact();
}

void YAGO::InitTaxonomy()
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
	adj.resize(N + 1);
	for (int i = 1; i <= N; i ++)
		adj[i].clear();

	int x, y;
	while (fscanf(subclassFile, "%d%d", &x, &y) == 2)
		adj[y].push_back(x);
}

void YAGO::InitType()
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

void YAGO::InitFact()
{

}

void YAGO::Traverse()
{
	int cur = M["owl:Thing"];
	vector<int> q; q.clear();
	q.push_back(cur);

	while (1)
	{
		cur = q[q.size() - 1];
		cout << "We are at : " << MM[cur] << endl << "Input your operation: " << endl;

		int x;
		string s;
		cin >> x;

		switch (x)
		{
			case 1 : 
				//print out the number of successors
				cout << "The number of successors: " << endl << adj[cur].size() << endl;
				break;
			case 2 : 
				//print all the successors
				cout << "The successors are: " << endl;
				for (int i = 0; i < adj[cur].size(); i ++)
					cout << MM[adj[cur][i]] << "     ";
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
				cout << "Sample entities are: " << endl;
				for (int i = 0; i < min((int) possess[cur].size(), 10); i ++)
					cout << "  " << EE[possess[cur][i]] << endl;
				break;
		}
		cout << "-------------------------------------" << endl << endl;
	}
}
