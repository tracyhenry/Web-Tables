#include "KB.h"
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <utility>
#include <iostream>
using namespace std;


KB::KB()
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
