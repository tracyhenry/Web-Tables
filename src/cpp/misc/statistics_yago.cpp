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

//queue
vector<int> q;
vector<int> used;
int f;

//dp
vector<int> opt;
vector<int> pick;

void dfs(int x)
{
	used[x] = 1;
	for (int i = 0; i < adj[x].size(); i ++)
		dfs(adj[x][i]);
}

void dp(int x)
{
	if (used[x]) return;
	used[x] = opt[x] = 0;
	for (int i = 0; i < adj[x].size(); i ++)
	{
		dp(adj[x][i]);
		if (opt[adj[x][i]] > opt[x])
		{
			opt[x] = max(opt[x], opt[adj[x][i]]);
			pick[x] = i;
		}
	}
	opt[x] ++;
	return ;
}

int main()
{
	freopen("D:\\Applications\\PortableGit-1.8.4-preview20130916\\Web-Tables\\data\\Knowledge Base\\yagoTaxonomy.ttl", "r", stdin);
	freopen("D:\\Applications\\PortableGit-1.8.4-preview20130916\\Web-Tables\\data\\Knowledge Base\\stats.txt", "a", stdout);
	
	string s;
	lines.clear();

	//basic statistics about the raw data
	int numberSubclass = 0;
	int totalLine = 0;
	int otherRelationship = 0;
	M.clear();
	edges.clear();
	
	while (getline(cin, s))
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

//	cout << "Total number of lines: " << endl << "       " << totalLine << endl;
//	cout << "Total number of subclass relationships: " << endl << "       " << numberSubclass << endl;
//	cout << "Total number of other relationships: " << endl << "       " << otherRelationship << endl;
	
	//make ids
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
	
	//statistics about the hierarchy
	int numberRoot = 0;
	for (int i = 1; i <= N; i ++)
		if (inDegree[i] == 0)
			numberRoot ++;
//	cout << "Total number of concepts: " << endl << "       " << N << endl;
//	cout << "Total number of possible roots: " << endl << "       " << numberRoot << endl;
	
	//topo sort
	q.clear();
	for (int i = 1; i <= N; i ++)
		if (inDegree[i] == 0)
			q.push_back(i);

	int f = 0;
	while (f < q.size())
	{
		int x = q[f ++];
		for (int i = 0; i < adj[x].size(); i ++)
		{
			int j = adj[x][i];
			inDegree[j] --;
			if (inDegree[j] == 0)
				q.push_back(j);
		}
	}
	
//	cout << q.size() << " " << N << endl;
//	if (q.size() != N)
//		cout << "There is Cycle in this knowledge graph!!" << endl; 


	//Depth First Search
	used.resize(N + 1);
	for (int i = 1; i <= N; i ++)
		used[i] = 0;
	
	dfs(M["owl:Thing"]);

	int maxDegree = 0;	
	int noThing = 0;
	int numberEdges = 0;
	
	for (int i = 1; i <= N; i ++)
		if (! used[i])
			noThing ++;
		else 
		{
			numberEdges += adj[i].size();
			maxDegree = max(maxDegree, (int) adj[i].size());
		}
	
//	cout << "The number of the maximum out degree : " << endl << "       " << maxDegree << endl;
//	cout << "Total number of concepts that are a subclass of owl:Thing: " << endl << "       " << N - noThing << endl;
//	cout << "Total number of extra edges: " << endl << "       " << numberEdges - (N - noThing - 1) << endl;

	//The longest directed paths
	used.resize(N + 1);
	for (int i = 1; i <= N; i ++)
		used[i] = 0;
	
	opt.resize(N + 1);
	pick.resize(N + 1);
	dp(M["owl:Thing"]);
	
	int cur = M["owl:Thing"];
	while (1)
	{
		if (MM[cur] != "owl:Thing")
			cout << " --> ";
		cout << MM[cur];
		if (adj[cur].size() == 0)
			break;
		cur = adj[cur][pick[cur]];
	}
//	cout << "The length of the longest directed path: " << endl << "       " << opt[M["owl:Thing"]];
	
	return 0;
}


















