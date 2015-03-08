#include <vector>
#include <list>
#include <map>
#include <set>
#include <deque>
#include <stack>
#include <bitset>
#include <cstring>
#include <string>
#include <algorithm>
#include <functional>
#include <numeric>
#include <utility>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <ctime>
#define LL unsigned long long
#define pi 3.1415926535897932384626433 
#define sqr(a) ((a)*(a))

using namespace std;

//raw data
vector<string> lines;
vector<pair<string, string> > edges;
map<string, int> M;
map<int, string> MM;

//graph
vector<vector<int> > adj;
vector<int> inDegree;
int N;

int main()
{
	ifstream fin("D:\\Applications\\PortableGit-1.8.4-preview20130916\\Web-Tables\\data\\KB\\yagoTaxonomy.ttl");	
	
	string s;
	lines.clear();

	//basic statistics about the raw data
	int numberSubclass = 0;
	int totalLine = 0;
	int otherRelationship = 0;
	M.clear();
	edges.clear();
	
	while (getline(fin, s))
	{
		totalLine ++;
		if (s.substr(0, 2) == "#@")
			continue;
		
		if (s.find("rdfs:subClassOf") == string::npos)
			otherRelationship ++;
		else numberSubclass ++;
		
		//check format
		int numberLeftParenthesis = 0;
		int numberRightParenthesis = 0;
		for (int i = 0; i < s.size(); i ++)
			if (s[i] == '<')
				numberLeftParenthesis ++;
			else if (s[i] == '>')
				numberRightParenthesis ++;
		
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
	fin.close();

	//make ids
	N = 0;
	MM.clear();
	for (map<string, int>::iterator it = M.begin(); it != M.end(); it ++)
		it->second = ++ N, MM[N] = it->first;
	
	ofstream fout1("D:\\Applications\\PortableGit-1.8.4-preview20130916\\Web-Tables\\data\\KB\\Concepts.txt");
	for (int i = 1; i <= N; i ++)
		fout1 << MM[i] << endl;	
	fout1.close();
	
	ofstream fout2("D:\\Applications\\PortableGit-1.8.4-preview20130916\\Web-Tables\\data\\KB\\SubClass.txt");
	for (int i = 0; i < edges.size(); i ++)
	{
		int x = M[edges[i].first];
		int y = M[edges[i].second];
		fout2 << x << " " << y << endl;
	}
	fout2.close();

	return 0;
}


















