#include "KB.h"
#include "Corpus.h"
#include <map>
#include <vector>
#include <string>
#include <iostream>
using namespace std;

KB *yago = new YAGO();
Corpus *wwt = new WWT();

int main()
{
	yago->Traverse();
	cout << wwt->countMultiColumnTable() << endl;
	return 0;
}
