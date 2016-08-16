#include "Experiment.h"
#include "Matcher.h"
#include <map>
#include <ctime>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <unordered_set>
using namespace std;

Bridge *bridge;

void interactiveQuery()
{
	while (1)
	{
		cout << endl << "-----------------------------------------" << endl;
		cout << "Input table_id and row_id :" << endl;
		int t, x, y;
		cin >> t;
		switch (t)
		{
			case 0:
				cin >> x >> y;
				bridge->findRecordConcept(x, y, 20, true);
				break;
			case 1:
				cin >> x >> y;
				bridge->findRelation(x, y, true);
				break;
			case 2:
				cin >> x >> y;
				bridge->findColConceptMajority(x, y, true);
				break;
			case 3:
				cin >> x;
				bridge->baselineFindColConceptAndRelation(x, true);
				break;
			case 4:
				cin >> x;
				bridge->kataraFindColConceptAndRelation(x, true);
				break;
			case 5:
				cin >> x;
				bridge->findColConceptAndRelation(x, true);
				break;
			case 6:
				cin >> x >> y;
				bridge->baselineFindRecordConcept(x, y, 20, true);
				break;
		}
		cout << endl << "-----------------------------------------" << endl;
	}
}

int main()
{
	bridge = new Bridge();
//	bridge->letsDebug();
	Experiment* experiment = new Experiment(bridge);
	experiment->runAllExp();
	bridge->enrichKB();
	interactiveQuery();

	return 0;
}
