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
	ifstream typeFile("D:\\Applications\\PortableGit-1.8.4-preview20130916\\Web-Tables\\data\\KB\\yagoTypes.ttl");
	ifstream conceptFile("D:\\Applications\\PortableGit-1.8.4-preview20130916\\Web-Tables\\data\\KB\\Concepts.txt");
	
	FILE *entityFile = fopen("D:\\Applications\\PortableGit-1.8.4-preview20130916\\Web-Tables\\data\\KB\\Entities.txt", "w");
	FILE *newTypeFile = fopen("D:\\Applications\\PortableGit-1.8.4-preview20130916\\Web-Tables\\data\\KB\\Types.txt", "w");

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
		{
			fakeConcept ++;
			if (fakeConcept <= 10)
				cout << s3 << endl;
		}
			
		entities[s1] = 0;
		types.push_back(make_pair(s1, M[s3]));
	}
	typeFile.close();
	
	cout << "Read the type file!" << endl;
	
	//write the entity file
	N = 0;
	for (map<string, int>::iterator it = entities.begin(); it != entities.end(); it ++)
		it->second = ++ N, fprintf(entityFile, "%s\n", (it->first).c_str());
	fclose(entityFile);
	
	cout << "Wrote the entity file!" << endl;
	
	//write the type file
	for (int i = 0; i < types.size(); i ++)
		fprintf(newTypeFile, "%d %d\n", entities[types[i].first], types[i].second);
	fclose(newTypeFile);
	
	cout << "Wrote the type file!" << endl << endl;
	
	cout << "notType: " << notType << endl;
	cout << "fakeConcept: " << fakeConcept << endl;
	
	system("pause");
	return 0;
}
