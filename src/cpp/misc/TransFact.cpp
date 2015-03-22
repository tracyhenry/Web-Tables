#include <set>
#include <map>
#include <cctype>
#include <vector>
#include <string>
#include <cstring>
#include <utility>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
using namespace std;

int main()
{
	ifstream fin("../../../data/KB/yagoFacts.ttl");
	ifstream entityFile("../../../data/KB/Entities.txt");

	ofstream relationFile("../../../data/KB/Relations.txt");
	ofstream factFile("../../../data/KB/Facts.txt");

	string s;
	int entityID = 0;
	int totalRelations = 0;
	map<string, int> rels; rels.clear();
	map<string, int> E; E.clear();
	vector<int> X, Y; X.clear(), Y.clear();
	vector<string> Z; Z.clear();

	while (getline(entityFile, s))
		E[s] = ++ entityID;

	while (getline(fin, s))
	{
		if (s.size() < 2 || s.substr(0, 2) == "#@")
			continue;
		stringstream sin(s);
		string s1, s2, s3;
		sin >> s1 >> s2 >> s3;

		//check
		if (s1[0] != '<' || s2[0] != '<' || s3[0] != '<')
			cout << s1 << " " << s2 << " " << s3 << endl;

		//remove brackets
		if (s1[0] == '<') s1 = s1.substr(1);
		if (s1[s1.size() - 1] == '>') s1.erase(s1.size() - 1, 1);

		if (s2[0] == '<') s2 = s2.substr(1);
		if (s2[s2.size() - 1] == '>') s2.erase(s2.size() - 1, 1);

		if (s3[0] == '<') s3 = s3.substr(1);
		if (s3[s3.size() - 1] == '>') s3.erase(s3.size() - 1, 1);

		//normalize
		string sx = "", sy = "";
		for (int i = 0; i < s1.size(); i ++)
		{
			if (! isascii(s1[i]))
				continue;
			if (s1[i] >= 65 && s1[i] <= 90)
				s1[i] = char(s1[i] + 32);
			sx += s1[i];
		}
		for (int i = 0; i < s3.size(); i ++)
		{
			if (! isascii(s3[i]))
				continue;
			if (s3[i] >= 65 && s3[i] <= 90)
				s3[i] = char(s3[i] + 32);
			sy += s3[i];
		}

		if (! E.count(sx) || ! E.count(sy))
		{
			if (s2 != "hasWebsite" && s2 != "hasMusicalRole"
				&& s2 != "imports" && s2 != "exports")
				cout << s1 << " " << s2 << " " << s3 << endl;
			continue;
		}

		rels[s2] = 0;
		int x = E[sx];
		int y = E[sy];

		Z.push_back(s2);
		X.push_back(x);
		Y.push_back(y);
	}

	for (map<string, int>::iterator it = rels.begin(); it != rels.end(); it ++)
		it->second = ++ totalRelations, relationFile << it->first << endl;

	if (X.size() != Y.size() || X.size() != Z.size())
		cout << "Wrong!!!!!!" << endl;

	for (int i = 0; i < X.size(); i ++)
		factFile << X[i] << " " <<  rels[Z[i]] << " " << Y[i] << endl;

	return 0;
}

