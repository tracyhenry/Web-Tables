#include "KB.h"
#include <map>
#include <cctype>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <utility>
#include <iostream>
#include <algorithm>
using namespace std;


KB::KB() :
	dirPath("../../../data/KB/"), delim("/")
{}

int KB::countConcept()
{
	return N;
}

int KB::countEntity()
{
	return K;
}

int KB::countRelation()
{
	return F;
}

int KB::getRoot()
{
	return root;
}

int KB::getPreCount(int conceptId)
{
	return pre[conceptId].size();
}

int KB::getPreNode(int conceptId, int index)
{
	return pre[conceptId][index];
}

int KB::getSucCount(int conceptId)
{
	return suc[conceptId].size();
}

int KB::getSucNode(int conceptId, int index)
{
	return suc[conceptId][index];
}

int KB::getBelongCount(int entityId)
{
	return belongs[entityId].size();
}

int KB::getBelongConcept(int entityId, int index)
{
	return belongs[entityId][index];
}

int KB::getPossessCount(int conceptId)
{
	return possess[conceptId].size();
}

int KB::getPossessEntity(int conceptId, int index)
{
	return possess[conceptId][index];
}

int KB::getRecursivePossessCount(int conceptId)
{
	return recursivePossess[conceptId].size();
}

unordered_set<int>& KB::getRecursivePossessEntities(int conceptId)
{
	return recursivePossess[conceptId];
}

int KB::getFactCount(int entityId)
{
	return facts[entityId].size();
}

string KB::toLower(string s)
{
	string ans = s;
	for (int i = 0; i < (int) ans.size(); i ++)
		if (isalpha(ans[i]) && ans[i] < 97)
			ans[i] += 32;
	return ans;
}

int KB::getConceptId(string c)
{
	return M[c];
}

int KB::getEntityId(string e)
{
	return E[e];
}

int KB::getRelationId(string r)
{
	return R[r];
}

int KB::getReverseRelationId(int relationId)
{
	if (relationId <= F / 2)
		return relationId + F / 2;
	else
		return relationId - F / 2;
}

bool KB::checkBelong(int entityId, int conceptId)
{
	for (int i = 0; i < (int) belongs[entityId].size(); i ++)
		if (belongs[entityId][i] == conceptId)
			return true;
	return false;
}

bool KB::checkRecursiveBelong(int entityId, int conceptId)
{
	for (int i = 0; i < (int) belongs[entityId].size(); i ++)
		if (isDescendant(belongs[entityId][i], conceptId))
			return true;
	return false;
}

bool KB::isDescendant(int ch, int fa)
{
	return startTime[fa] <= startTime[ch] && endTime[ch] <= endTime[fa];
}

pair<int, int> KB::getFactPair(int entityId, int index)
{
	return facts[entityId][index];
}

string KB::getConcept(int conceptId)
{
	return MM[conceptId];
}

string KB::getEntity(int entityId)
{
	return EE[entityId];
}

string KB::getRelation(int relationId)
{
	return RR[relationId];
}

int KB::getDepth(int x)
{
	return depth[x];
}

int KB::getLevel(int x)
{
	return level[x];
}

int KB::getRelTripleCount(int relationId)
{
	return relTripleCount[relationId];
}

int KB::getEntPairTripleCount(int e1, int e2)
{
	return entPairTripleCount[e1][e2];
}

void KB::initTaxonomy()
{

	//YAGO's Input File
	ifstream conceptFile(conceptFileName.c_str());
	FILE *subclassFile = fopen(subclassFileName.c_str(), "r");

	//make the maps
	M.clear();
	MM.clear();
	N = 0;

	string c;
	while (getline(conceptFile, c))
		M[c] = ++ N, MM[N] = c;
	conceptFile.close();

	//make graph
	pre.clear(), suc.clear();
	pre.resize(N + 1), suc.resize(N + 1);
	for (int i = 1; i <= N; i ++)
		pre[i].clear(), suc[i].clear();

	int x, y;
	while (fscanf(subclassFile, "%d%d", &x, &y) == 2)
		pre[x].push_back(y), suc[y].push_back(x);

	root = M["owl:Thing"];
}

void KB::initType()
{
	ifstream entityFile(entityFileName.c_str());
	FILE *typeFile = fopen(typeFileName.c_str(), "r");

	//Get entity strings
	E.clear();
	EE.clear();
	K = 0;
	string e;
	while (getline(entityFile, e))
		E[e] = ++ K, EE[K] = e;

	//make relationship
	int x, y;
	possess.clear(), belongs.clear();
	possess.resize(N + 1);
	belongs.resize(K + 1);
	for (int i = 1; i <= N; i ++)
		possess[i].clear();
	for (int i = 1; i <= K; i ++)
		belongs[i].clear();
	while (fscanf(typeFile, "%d%d", &x, &y) == 2)
		belongs[x].push_back(y), possess[y].push_back(x);

	//dfs
	depth.clear(), level.clear();
	startTime.clear(), endTime.clear();
	recursivePossess.clear();
	depth.resize(N + 1), level.resize(N + 1);
	startTime.resize(N + 1), endTime.resize(N + 1);
	recursivePossess.resize(N + 1);
	timeStamp = 0; level[root] = 0;
	doDFS(root);
}

void KB::initFact()
{
	ifstream relationFile(relationFileName.c_str());
	FILE *factFile = fopen(factFileName.c_str(), "r");

	//Get relation names
	R.clear();
	RR.clear();
	F = 0;
	string r;
	while (getline(relationFile, r))
		R[r] = ++ F, RR[F] = r;

	//Reverse relations
	for (int i = F + 1; i <= F * 2; i ++)
	{
		string curRelation = "** " + RR[i - F] + " **";
		R[curRelation] = i, RR[i] = curRelation;
	}
	F *= 2;

	//Get facts
	int x, z, y;
	facts.clear();
	facts.resize(K + 1);
	for (int i = 1; i <= K; i ++)
		facts[i].clear();
	relTripleCount.clear();
	relTripleCount.resize(F + 1);
	entPairTripleCount.clear();
	entPairTripleCount.resize(K + 1);
	while (fscanf(factFile, "%d%d%d", &x, &z, &y) == 3)
	{
		facts[x].emplace_back(z, y);
		facts[y].emplace_back(z + F / 2, x);
		//for katara
		relTripleCount[z] ++;
		relTripleCount[z + F / 2] ++;
		entPairTripleCount[x][y] ++;
		entPairTripleCount[y][x] ++;
	}
}

void KB::doDFS(int x)
{
	depth[x] = 1;
	startTime[x] = ++ timeStamp;
	recursivePossess[x].insert(possess[x].begin(), possess[x].end());
	for (int i = 0; i < (int) suc[x].size(); i ++)
	{
		int j = suc[x][i];
		level[j] = level[x] + 1;
		doDFS(j);
		depth[x] = max(depth[x], depth[j] + 1);
		recursivePossess[x].insert(recursivePossess[j].begin(),	recursivePossess[j].end());
	}
	endTime[x] = ++ timeStamp;
}

YAGO::YAGO()
{
	conceptFileName = dirPath + "Concepts.txt";
	subclassFileName = dirPath + "SubClass_Tree.txt";
	entityFileName = dirPath + "Entities.txt";
	typeFileName = dirPath + "Types_Tree.txt";
	relationFileName = dirPath + "Relations.txt";
	factFileName = dirPath + "Facts.txt";

	cout << "Initializing YAGO!" << endl;

	initTaxonomy();
	initType();
	initFact();
	initSupFacts();
}

void YAGO::initSupFacts()
{
	int totalAdded = 0;

	//init lower M
	lowerM.clear();
	for (auto kv : M)
		lowerM[toLower(kv.first)] = kv.second;

	//playsFor
	int clubRoot = lowerM["wordnet_club_108227214"];
	unordered_set<int> &clubs = recursivePossess[clubRoot];
	cout << "Total number of clubs: " << endl << '\t' << clubs.size() << endl;
	for (int club : clubs)
	{
		string clubName = EE[club];
		string conceptName = "wikicategory_" + clubName + "_players";
		if (! lowerM.count(conceptName))
			continue;
		int conceptId = lowerM[conceptName];
		unordered_set<int> players = recursivePossess[conceptId];
		for (int player : players)
		{
			totalAdded += 2;
			int x = player, y = club, z = R["playsFor"];
			facts[x].emplace_back(z, y);
			facts[y].emplace_back(z + F / 2, x);
			//for katara
			relTripleCount[z] ++;
			relTripleCount[z + F / 2] ++;
			entPairTripleCount[x][y] ++;
			entPairTripleCount[y][x] ++;

			x = player, y = club, z = R["isAffiliatedTo"];
			facts[x].emplace_back(z, y);
			facts[y].emplace_back(z + F / 2, x);
			//for katara
			relTripleCount[z] ++;
			relTripleCount[z + F / 2] ++;
			entPairTripleCount[x][y] ++;
			entPairTripleCount[y][x] ++;
		}
	}

	//isCitizenOf
	unordered_map<string, string> demonymMap;
	string demonymFileName = dirPath + "Demonyms.txt";
	ifstream demonymFile(demonymFileName.c_str());
	string inputLine;
	while (demonymFile >> inputLine)
	{
		string country, demonym;
		int pos;
		//first tab
		pos = inputLine.find('\t');
		country = inputLine.substr(0, pos);
		country = toLower(country);
		inputLine.erase(0, pos + 1);
		//second tab
		pos = inputLine.find('\t');
		demonym = inputLine.substr(0, pos);
		demonym = toLower(demonym);

		demonymMap[demonym] = country;
		cout << country << '\t' << demonym << endl;
	}

	int personRoot = M["wordnet_person_100007846"];
	for (int i = 1; i <= N; i ++)
	{
		if (! isDescendant(i, personRoot))
			continue;
		string conceptName = toLower(MM[i]);
		string curCountry = "";
		for (auto kv : demonymMap)
			if (conceptName.find(kv.first) != string::npos)
			{
				curCountry = kv.second;
				break;
			}
		if (! E.count(curCountry))
			continue;
		cout << conceptName << " " << curCountry << endl;
		unordered_set<int> people = recursivePossess[i];
		for (int person : people)
		{
			bool hasCitizenship = false;
			for (auto kv : facts[person])
				if (kv.first == R["isCitizenOf"])
				{
					hasCitizenship = true; break;
				}
			if (hasCitizenship)
				continue;

			totalAdded ++;
			int x = person, y = E[curCountry], z = R["isCitizenOf"];
			facts[x].emplace_back(z, y);
			facts[y].emplace_back(z + F / 2, x);
			//for katara
			relTripleCount[z] ++;
			relTripleCount[z + F / 2] ++;
			entPairTripleCount[x][y] ++;
			entPairTripleCount[y][x] ++;
		}
	}

	cout << "Supplementary facts generated. Total number of added facts: "
		<< '\t' << totalAdded << endl;
}
