#include <iostream>
#include <cstdio>
#include <vector>
#include <cstring>
#include <string>
#include <sstream>
#include <map>
#include <set>
using namespace std;

int matched[1010100];

int main()
{
	freopen("../../../data/fuzzy/result_cosine_0.8_0.7.txt", "r", stdin);

	string s;
	memset(matched, 0, sizeof(matched));

	while (getline(cin, s))
	{
		//line 1
		stringstream sin1(s);
		double sim;
		int entityID, cellID;

		sin1 >> sim >> entityID >> cellID;

		//line 2~4
		for (int i = 1; i <= 3; i ++)
			getline(cin, s);

		matched[cellID] ++;
	}

	int multiMatch = 0;
	int noMatch = 0;
	int oneMatch = 0;
	for (int i = 0; i < 551351; i ++)
		if (matched[i] > 1)
			multiMatch ++;
		else if (matched[i] == 1)
			oneMatch ++;
		else noMatch ++;

	cout << "Number of cells that have more than one matched entity: " << endl << "      " << multiMatch << endl;
	cout << "Number of cells that have exactly one matched entity: " << endl << "      " << oneMatch << endl;
	cout << "Number of cells that have no matches: " << endl << "      " << noMatch << endl;
	return 0;
}
