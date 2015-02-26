#include "KB.h"
#include <map>
#include <vector>
#include <string>
#include <iostream>
using namespace std;

KB aha;

void dfs(int x)
{
	aha.used[x] = 1;
	for (int i = 0; i < aha.adj[x].size(); i ++)
		dfs(aha.adj[x][i]);
}

int main()
{
	aha.used.resize(aha.N + 1);
	for (int i = 1; i <= aha.N; i ++)
		aha.used[i] = 0;
	
	dfs(aha.M["owl:Thing"]);
	
	int noThing = 0;
	int numberEdges = 0;
	for (int i = 1; i <= aha.N; i ++)
		if (! aha.used[i])
			noThing ++;
		else numberEdges += aha.adj[i].size();
	
	cout << "Total number of concepts that are a subclass of owl:Thing: " << endl << "       " << aha.N - noThing << endl;
	cout << "Total number of extra edges: " << endl << "       " << numberEdges - (aha.N - noThing - 1) << endl;
	
	system("pause");
	return 0;
}
