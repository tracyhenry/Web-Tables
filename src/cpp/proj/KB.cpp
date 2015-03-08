#include "KB.h"
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <utility>
#include <iostream>
using namespace std;


YAGO::YAGO()
{
	InitTaxonomy();
}

void YAGO::InitTaxonomy()
{

	//YAGO's Input File
	ifstream conceptFile("D:\\Applications\\PortableGit-1.8.4-preview20130916\\Web-Tables\\data\\KB\\Concepts.txt");
	
	FILE *subclassFile = fopen("D:\\Applications\\PortableGit-1.8.4-preview20130916\\Web-Tables\\data\\KB\\SubClass.txt", "r");
	
	//some variables for reading the file
	M.clear();
	MM.clear();
	N = 0;
	
	//make the maps
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
				string s;
				cin >> s;
				q.push_back(M[s]);
				break;
		}
		cout << "-------------------------------------" << endl << endl;
	}
}
