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
	ifstream fin("D:\\Applications\\PortableGit-1.8.4-preview20130916\\Web-Tables\\data\\Knowledge Base\\yagoTaxonomy.ttl");
	//some variables for reading the file
	vector<pair<string, string> > edges;
	string s;
	
	//initialize containers
	edges.clear();
	M.clear();
	
	while (getline(fin, s))
	{
		if (s.substr(0, 2) == "#@")
			continue;
	
		stringstream ss(s);
		string s1, s2, s3;
		ss >> s1 >> s2 >> s3;	
	
		//remove the parenthesis
		if (s1[0] == '<') s1 = s1.substr(1);
		if (s1[s1.size() - 1] == '>')
			s1 = s1.substr(0, s1.size() - 1);

		if (s3[0] == '<') s3 = s3.substr(1);
		if (s3[s3.size() - 1] == '>')
			s3 = s3.substr(0, s3.size() - 1);
			
		edges.push_back(make_pair(s1, s3));
		M[s1] = M[s3] = 0;
	}
	
	N = 0;
	MM.clear();
	for (map<string, int>::iterator it = M.begin(); it != M.end(); it ++)
		it->second = ++ N, MM[N] = it->first;
	
	//make graph
	adj.resize(N + 1);
	inDegree.resize(N + 1);
	for (int i = 1; i <= N; i ++)
		inDegree[i] = 0, adj[i].clear();

	for (int i = 0; i < edges.size(); i ++)
	{
		int x = M[edges[i].first];
		int y = M[edges[i].second];
		adj[y].push_back(x);
		inDegree[x] ++;
	}
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
