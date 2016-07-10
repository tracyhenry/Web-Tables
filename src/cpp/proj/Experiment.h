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

};

#endif
