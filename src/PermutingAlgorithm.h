#ifndef KemenyYoung_PermutingAlgorithm
#define KemenyYoung_PermutingAlgorithm

#include "Algorithm.h"

class PermutingAlgorithm : public Algorithm
{
public:
	PermutingAlgorithm();
	virtual ~PermutingAlgorithm();

	virtual std::deque<std::set<ChoiceID> > Run(const Choices &choices, const std::deque<Voter> &voters);

private:
	struct Internals;
	Internals *intern;
};

#endif
