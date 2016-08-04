#include "Bridge.h"
#include "Matcher.h"
#include <iostream>
#include <algorithm>
#include <unordered_set>
#define IterID unordered_map<int, double>::iterator
#define IterIT unordered_map<int, TaxoPattern *>::iterator
using namespace std;

void Bridge::letsDebug()
{

	debug << "Debugging for katara relSC arrays..." << endl << endl;
	string conceptStr = "wordnet_actor_109765278";
	int curConcept = kb->getConceptId(conceptStr);
	debug << "Concept : " << conceptStr  << "  id : " << curConcept << endl;
	for (int i = 1; i <= kb->countRelation(); i ++)
		debug << kb->getRelation(i) << " : " << relSC[curConcept][i] << endl;
	debug << endl << endl;

	TaxoPattern *p0, *p1, *p2, *p3;

	debug << "Debugging for table with table_id = 4125..." << endl;
	p0 = getKbSchema(kb->getConceptId("wikicategory_Bridges_on_the_National_Register_of_Historic_Places_in_Georgia_(U.S._state)"),
			kb->getRelationId("isLocatedIn"), true);
	p1 = getKbSchema(kb->getConceptId("wikicategory_Road_bridges_in_Oregon"),
			kb->getRelationId("isLocatedIn"), true);
	p2 = getKbSchema(kb->getConceptId("wikicategory_Bridges_in_Portland,_Oregon"),
			kb->getRelationId("isLocatedIn"), true);
	p3 = cellPattern[302231];
	printPattern(p3);

	depthVector sim0 = Matcher::dVector(kb, p0, p3);
	depthVector sim1 = Matcher::dVector(kb, p1, p3);
	depthVector sim2 = Matcher::dVector(kb, p2, p3);

	debug << endl;
	debug << "Similarity for wikicategory_Bridges_on_the_National_Register_of_Historic_Places_in_Georgia_(U.S._state) : " << sim0.score(Matcher::M) << endl;
	debug << '\t';
	for (int i = (int) sim0.w.size() - 1; i >= 0; i --)
		debug << sim0.w[i] << " ";
	debug << endl << endl;
	debug << "Similarity for wikicategory_Road_bridges_in_Oregon : " << sim1.score(Matcher::M) << endl;
	debug << '\t';
	for (int i = (int) sim1.w.size() - 1; i >= 0; i --)
		debug << sim1.w[i] << " ";
	debug << endl << endl;
	debug << "Similarity for wikicategory_Bridges_in_Portland,_Oregon : " << sim2.score(Matcher::M) << endl;
	debug << '\t';
	for (int i = (int) sim2.w.size() - 1; i >= 0; i --)
		debug << sim2.w[i] << " ";
	debug << endl << endl;

	debug << "Debugging for table with table_id = 1356..." << endl;
	p0 = getKbSchema(kb->getConceptId("wikicategory_The_Strongest_managers"),
			kb->getRelationId("playsFor"), true);
	p1 = getKbSchema(kb->getConceptId("wikicategory_Argentine_expatriates_in_Austria"),
			kb->getRelationId("playsFor"), true);
	p2 = getKbSchema(kb->getConceptId("wikicategory_La_Liga_footballers"),
			kb->getRelationId("playsFor"), true);
	p3 = colPattern[corpus->getTableByDataId(1356).id][2];
	printPattern(p3);

	sim0 = Matcher::dVector(kb, p0, p3);
	sim1 = Matcher::dVector(kb, p1, p3);
	sim2 = Matcher::dVector(kb, p2, p3);

	debug << endl;
	debug << "Similarity for wikicategory_The_Strongest_managers : " << endl;
	debug << '\t';
	for (int i = (int) sim0.w.size() - 1; i >= 0; i --)
		debug << sim0.w[i] << " ";
	debug << endl << endl;
	debug << "Similarity for wikicategory_Argentine_expatriates_in_Austria : " << endl;
	debug << '\t';
	for (int i = (int) sim1.w.size() - 1; i >= 0; i --)
		debug << sim1.w[i] << " ";
	debug << endl << endl;
	debug << "Similarity for wikicategory_La_Liga_footballers : " << endl;
	debug << '\t';
	for (int i = (int) sim2.w.size() - 1; i >= 0; i --)
		debug << sim2.w[i] << " ";
	debug << endl << endl;

	debug << "Debugging for table with table_id = 3297..." << endl;
	int rel = kb->getRelationId("created");
	int reverseRel = kb->getReverseRelationId(rel);
	int owlThingId = kb->getConceptId("owl:Thing");
	int albumId = kb->getConceptId("wordnet_album_106591815");

	debug << "created relationId : " << rel << endl << "** created ** relationId : " << reverseRel << endl
		<< "owl:Thing conceptId : " << owlThingId << endl
		<< "album conceptId : " << albumId << endl;

	p0 = getKbSchema(albumId, reverseRel, true);
	p1 = getKbSchema(owlThingId, reverseRel, true);
	p2 = colPattern[corpus->getTableByDataId(3297).id][3];
	printPattern(p2);

	sim0 = Matcher::dVector(kb, p0, p2);
	sim1 = Matcher::dVector(kb, p1, p2);

	debug << endl << endl;
	debug << "Similarity for albums : " << endl << '\t';
	for (int i = (int) sim0.w.size() - 1; i >= 0; i --)
		debug << sim0.w[i] << " ";
	debug << endl << endl;
	debug << "Similarity for owl:Thing : " << endl << '\t';
	for (int i = (int) sim1.w.size() - 1; i >= 0; i --)
		debug << sim1.w[i] << " ";
	debug << endl;

	debug << "Debugging for table with table_id = 2843..." << endl;
	rel = kb->getRelationId("actedIn");
	reverseRel = kb->getReverseRelationId(rel);
	owlThingId = kb->getConceptId("owl:Thing");
	int actorId = kb->getConceptId("wordnet_actor_109765278");
	int peopleId = kb->getConceptId("wikicategory_Living_people");

	debug << "actedIn relationId : " << rel << endl
			<< "owl:Thing conceptId : " << owlThingId << endl
			<< "actor conceptId : " << actorId << endl
			<< "people conceptId : " << peopleId << endl;

	p0 = getKbSchema(actorId, rel, true);
	p1 = getKbSchema(peopleId, rel, true);
	p2 = colPattern[corpus->getTableByDataId(2843).id][2];
	printPattern(p2);

	sim0 = Matcher::dVector(kb, p0, p2);
	sim1 = Matcher::dVector(kb, p1, p2);

	debug << endl << endl;
	debug << "Similarity for actor : " << endl << '\t';
	for (int i = (int) sim0.w.size() - 1; i >= 0; i --)
			debug << sim0.w[i] << '\t';
	debug << endl << endl;
	debug << "Similarity for living people : " << endl << '\t';
	for (int i = (int) sim1.w.size() - 1; i >= 0; i --)
			debug << sim1.w[i] << '\t';
	debug << endl;

	debug << "Debugging for table with table_id = 3317..." << endl;
	rel = kb->getRelationId("isLocatedIn");
	reverseRel = kb->getReverseRelationId(rel);
	int areaId = kb->getConceptId("wordnet_geographical_area_108574314");
	int villageId = kb->getConceptId("wordnet_village_108672738");

	debug << "isLocatedIn relationId : " << rel << endl
			<< "area conceptId : " << areaId << endl
			<< "village conceptId : " << villageId << endl;

	p0 = getKbSchema(areaId, rel, true);
	p1 = getKbSchema(villageId, rel, true);
	p2 = colPattern[corpus->getTableByDataId(3317).id][1];
	printPattern(p2);

	sim0 = Matcher::dVector(kb, p0, p2);
	sim1 = Matcher::dVector(kb, p1, p2);

	debug << endl << endl;
	debug << "Similarity for area : " << endl << '\t';
	for (int i = (int) sim0.w.size() - 1; i >= 0; i --)
			debug << sim0.w[i] << '\t';
	debug << endl << endl;
	debug << "Similarity for village : " << endl << '\t';
	for (int i = (int) sim1.w.size() - 1; i >= 0; i --)
			debug << sim1.w[i] << '\t';
	debug << endl;
}

TaxoPattern *Bridge::getKbSchema(int conceptId, int relationId, bool isDebug)
{
	TaxoPattern *ans = conSchema[conceptId][relationId];
	if (isDebug)
	{
		for (int i = 1; i <= 10; i ++)
			debug << endl;
		debug <<  "-------------------------------------------" << endl;
		debug << conceptId << " " << kb->getConcept(conceptId) << " " <<
			kb->getRelation(relationId) << " : " << endl;
		debug <<  "-------------------------------------------" << endl;

		if (ans != NULL)
			printPattern(ans);
	}
	return ans;
}

TaxoPattern *Bridge::getCellPattern(int cellId, bool isDebug)
{
	TaxoPattern *ans = cellPattern[cellId];
	if (isDebug)
	{
		for (int i = 1; i <= 10; i ++)
			debug << endl;
		debug << "-----------------------------------------" << endl;
		debug << "The pattern of cell id : " << cellId << endl;
		debug << "-----------------------------------------" << endl;

		//sort array
		printPattern(ans);
	}
	return ans;
}

void Bridge::printPattern(TaxoPattern *p)
{
	int testCid = 0;
	unordered_map<int, double> &C = p->c;

	//concepts
	vector<pair<double, int>> tmp; tmp.clear();
	for (IterID it = C.begin(); it != C.end(); it ++)
		if (! testCid || cellPattern[testCid]->c.count(it->first))
			tmp.emplace_back(- it->second, it->first);

	debug << endl << "Total Entity : " << p->numEntity << endl;
	sort(tmp.begin(), tmp.end());
	for (int i = 0; i < (int) tmp.size(); i ++)
		debug << tmp[i].second << " " << kb->getConcept(tmp[i].second)
			<< " : " << - tmp[i].first
			<< " " << kb->getDepth(tmp[i].second) << endl;

	//entities
	unordered_map<int, double> &E = p->e;
	for (IterID it = E.begin(); it != E.end(); it ++)
		if (! testCid || cellPattern[testCid]->e.count(it->first))
			debug << kb->getEntity(it->first) << " : " << it->second << endl;
}

void Bridge::testPattern()
{
	int totalEntity = kb->countEntity();
	int maxLeafPatternSize = 0;
	int sumLeafPatternSize = 0;
	int maxPatternSize = 0;
	int sumPatternSize = 0;

	for (int i = 1; i <= totalEntity; i ++)
	{
		unordered_set<int> anc;
		anc.clear();

		int totalBelong = kb->getBelongCount(i);
		for (int j = 0; j < totalBelong; j ++)
		{
			int cid = kb->getBelongConcept(i, j);
			while (kb->getPreCount(cid))
			{
				anc.insert(cid);
				cid = kb->getPreNode(cid, 0);
			}
		}
		int curPatternSize = 0;
		for (unordered_set<int>::iterator it = anc.begin(); it != anc.end(); it ++)
			curPatternSize ++;
		maxPatternSize = max(maxPatternSize, curPatternSize);
		sumPatternSize += curPatternSize;
		maxLeafPatternSize = max(maxLeafPatternSize, totalBelong);
		sumLeafPatternSize += totalBelong;
	}

	cout << "Maximum leaf size of a pattern: " << endl << "      "
		<< maxLeafPatternSize << endl;
	cout << "Average leaf size of a pattern: " << endl << "      "
		<< (int) ((double) sumLeafPatternSize / totalEntity) << endl;
	cout << "Maximum size of a pattern: " << endl << "      "
		<< maxPatternSize << endl;
	cout << "Average size of a pattern: " << endl << "      "
		<< (int) ((double) sumPatternSize / totalEntity) << endl;
}
