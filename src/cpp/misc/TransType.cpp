#include <cctype>
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

int main()
{
	ifstream typeFile("../../../data/KB/yagoTypes.ttl");
	ifstream conceptFile("../../../data/KB/Concepts.txt");

	ofstream entityFile("../../../data/KB/Entities.txt");
	ofstream newTypeFile("../../../data/KB/Types.txt");

	//Get concepts
	map<string, int> M;	M.clear();
	string c;
	int N;

	while (getline(conceptFile, c))
		M[c] = ++ N;
	conceptFile.close();

	cout << "Got concepts!" << endl;

	//Deal with type file
	map<string, int> entities; entities.clear();
	vector<pair<string, int> > types; types.clear();
	string s;
	int lineID = 0;
	int nEntity = 0;
	int notType = 0;
	int fakeConcept = 0;


	while (getline(typeFile, s))
	{
		lineID ++;
		if (lineID < 20 || lineID % 2 == 1)
			continue;

		stringstream ss(s);
		string s1, s2, s3;
		ss >> s1 >> s2 >> s3;

		if (s1[0] == '<') s1 = s1.substr(1);
		if (s1[s1.size() - 1] == '>')
			s1 = s1.substr(0, s1.size() - 1);

		if (s3[0] == '<') s3 = s3.substr(1);
		if (s3[s3.size() - 1] == '>')
			s3 = s3.substr(0, s3.size() - 1);

		if (s2 != "rdf:type")
			notType ++;

		if (! M.count(s3))
			fakeConcept ++;

		//check s1 with ascii
		string norm_s1 = "";
		for (int i = 0; i < s1.size(); i ++)
		{
			if (! isascii(s1[i]))
				continue;
			if (s1[i] >= 65 && s1[i] <= 90)
				s1[i] = char(s1[i] + 32);
			norm_s1 += s1[i];
		}
		if (norm_s1 == "")
			continue;
		entities[norm_s1] = 0;
		types.push_back(make_pair(norm_s1, M[s3]));
	}

	typeFile.close();

	cout << "Read the type file!" << endl;

	//write the entity file
	N = 0;
	for (map<string, int>::iterator it = entities.begin(); it != entities.end(); it ++)
		it->second = ++ N, entityFile << it->first << endl;
	entityFile.close();

	cout << "Wrote the entity file!" << endl;

	//write the type file
	set<pair<int, int>> typeSet; typeSet.clear();
	for (int i = 0; i < types.size(); i ++)
	{
		int x = entities[types[i].first];
		int y = types[i].second;
		if (x == 468539)
			cout << y << endl;
		if (typeSet.count(make_pair(x, y)))
			continue;

		newTypeFile << x << " " << y << endl;
		typeSet.insert(make_pair(x, y));
	}
	newTypeFile.close();

	cout << "Wrote the type file!" << endl << endl;

	cout << "notType: " << notType << endl;
	cout << "fakeConcept: " << fakeConcept << endl;

	return 0;
}
