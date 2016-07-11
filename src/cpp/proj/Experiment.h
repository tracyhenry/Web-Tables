#ifndef EXP_H
#define EXP_H
#include "Bridge.h"
#include <string>


class Experiment
{
private:
	Bridge *bridge;

	//a function checking whether two string representing the same concept
	bool isEqualConcept(std::string, std::string);

public:

	//Constructor
	Experiment() {}
	Experiment(Bridge *);

	//run a set of experiments
	void runAllExp();

	//column concept naive method
	void runExpColConceptNaive();

	//test the running time of finding relation
	void runColRelationLatency();

	//test the running time of find record concept
	void runRecConceptLatency();

};

#endif
