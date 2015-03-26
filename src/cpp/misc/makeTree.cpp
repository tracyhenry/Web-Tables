#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
using namespace std;

vector<vector<int>> adj;
vector<string> concepts;
vector<bool> underThing;
vector<int> q;
int root, f;

int main()
{
	ifstream fin1("../../../data/KB/SubClass.txt");
	ifstream fin2("../../../data/KB/Types.txt");
	ifstream fin3("../../../data/KB/Concepts.txt");

	ofstream fout1("../../../data/KB/SubClass_Tree.txt");
	ofstream fout2("../../../data/KB/Types_Tree.txt");

	adj.resize(1010100);
	for (int i = 1; i <= 1000000; i ++)
		adj[i].clear();

	//read in the subclass file
	int N = 0;
	int x, y;
	while (fin1 >> x >> y)
		adj[y].push_back(x);

	//find the root: owl:Thing
	string s;
	concepts.clear();
	concepts.push_back("");
	while (getline(fin3, s))
		concepts.push_back(s), N ++;
	for (int i = 1; i <= N; i ++)
		if (concepts[i] == "owl:Thing")
			root = i;
	cout << concepts[426058] << endl;
	cout << "N: " << endl << "      " << N << endl;
	cout << "Root number is : " << endl << "      " << root << endl;

	//bfs
	underThing.clear();
	underThing.resize(N + 1);
	q.clear();
	q.push_back(root);
	underThing[root] = true;
	f = 0;

	while (f < q.size())
	{
		int x = q[f ++];
		for (int i = 0; i < adj[x].size(); i ++)
		{
			int j = adj[x][i];
			if (! underThing[j])
			{
				q.push_back(j);
				underThing[j] = true;
				fout1 << j << " " << x << endl;
			}
		}
	}
	fout1.close();

	cout << "Number of underThing: " << endl << "      " << q.size() << endl;

	//affect
	int totalType = 0;
	int affectedType = 0;
	while (fin2 >> x >> y)
	{
		totalType ++;
		if (! underThing[y])
			affectedType ++;
		else
			fout2 << x << " " << y << endl;
	}
	fout2.close();

	cout << "Total types: " << endl << "      " << totalType << endl;
	cout << "Affected types: " << endl << "      " << affectedType << endl;

	return 0;
}
