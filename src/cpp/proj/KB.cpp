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
	dirPath("D:\\Applications\\PortableGit-1.8.4-preview20130916\\"), delim("\\") 
{}


YAGO::YAGO()
{
	InitTaxonomy();
	InitType();
}

void YAGO::InitTaxonomy()
{
	
	//YAGO's Input File
	ifstream conceptFile((dirPath + "Web-Tables" + delim + "data" + delim + "KB" + delim + "Concepts.txt").c_str());

	FILE *subclassFile = fopen((dirPath + "Web-Tables" + delim + "data" + delim + "KB" + delim + "SubClass.txt").c_str(), "r");
	
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
	ifstream entityFile((dirPath + "Web-Tables" + delim + "data" + delim + "KB" + delim + "Entities.txt").c_str());
	
	FILE *typeFile = fopen((dirPath + "Web-Tables" + delim + "data" + delim + "KB" + delim + "Types.txt").c_str(), "r");
	
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
