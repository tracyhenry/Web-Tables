#include "KB.h"
#include <map>
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

int KB::getFactCount(int entityId)
{
	return facts[entityId].size();
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

bool KB::checkBelong(int entityId, int conceptId)
{
	for (int i = 0; i < belongs[entityId].size(); i ++)
		if (belongs[entityId][i] == conceptId)
			return true;
	return false;
}

bool KB::checkRecursiveBelong(int entityId, int conceptId)
{
	for (int i = 0; i < belongs[entityId].size(); i ++)
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

	//dfs
	depth.clear();
	depth.resize(N + 1);
	startTime.resize(N + 1);
	endTime.resize(N + 1);
	timeStamp = 0;
	doDFS(root);
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
	while (fscanf(factFile, "%d%d%d", &x, &z, &y) == 3)
	{
		facts[x].emplace_back(z, y);
		facts[y].emplace_back(z + F / 2, x);
	}
}

void KB::doDFS(int x)
{
	depth[x] = 1; 
	startTime[x] = ++ timeStamp;
	for (int i = 0; i < suc[x].size(); i ++)
	{
		int j = suc[x][i];
		doDFS(j);
		depth[x] = max(depth[x], depth[j] + 1);
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
}

void YAGO::getConceptWithMostFacts()
{
	vector<pair<int, string>> mmm;
	mmm.clear();
	for (int i = 1; i <= N; i ++)
	{
		if (MM[i].substr(0, 4) != "word")
			continue;
		int tot = 0;
		for (int j = 0; j < possess[i].size(); j ++)
		{
			int x = possess[i][j];
			tot += facts[x].size();
		}
		mmm.emplace_back(-tot, MM[i]);
	}
	sort(mmm.begin(), mmm.end());
	for (int i = 0; i < 20; i ++)
		cout << mmm[i].second << " : " << -mmm[i].first << endl;
}
