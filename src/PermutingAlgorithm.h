#ifndef KemenyYoung_PermutingAlgorithm
#define KemenyYoung_PermutingAlgorithm

#include "Algorithm.h"

class PermutingAlgorithm : public Algorithm
{
public:
	PermutingAlgorithm(const Choices &choices, const std::deque<Voter> &voters);
	virtual ~PermutingAlgorithm();

protected:
	virtual std::deque<std::set<ChoiceID> > Execute(const std::set<ChoiceID> &choices, const std::deque<Voter> &voters);

private:
	struct Internals;
	Internals *intern;
};

#endif
