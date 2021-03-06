#ifndef EXP_H
#define EXP_H
#include "Bridge.h"
#include <vector>
#include <string>


class Experiment
{
private:
	Bridge *bridge;

	//a function checking whether two string representing the same concept
	bool isEqualConcept(std::string, std::string);

	//a scoring function for an output concept based on its steps
	double getScore(std::string, std::string);

	//a function to calculate precision, recall and f value
	std::vector<double> calculatePRF(double, double, double, bool);

public:

	//Constructor
	Experiment() {}
	Experiment(Bridge *);

	//run a set of experiments
	void runAllExp();

	//column concept experiments
	std::vector<double> runExpColConcept(std::string, bool);

	//column relation experiments
	std::vector<double> runExpColRelation(std::string, bool);

	//record concept experiments
	std::vector<double> runExpRecConcept(std::string, bool);

	//Parameter testing for col concept & relation
	void runExpColConceptAndRelationParam();

	//test the running time of finding relation
	void runColRelationLatency();

	//test the running time of find record concept
	void runRecConceptLatency();

};

#endif
