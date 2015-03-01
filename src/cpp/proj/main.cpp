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
	
	system("pause");
	return 0;
}
