#include "KB.h"
#include <map>
#include <vector>
#include <string>
#include <iostream>
using namespace std;

KB *aha = new YAGO();

int main()
{
	aha->Traverse();
	return 0;
}
