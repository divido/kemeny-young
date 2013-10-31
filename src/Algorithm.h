#ifndef KemenyYoung_Algorithm
#define KemenyYoung_Algorithm

#include <string>
#include <deque>
#include <set>

#include "Choices.h"
#include "Voter.h"

class Algorithm
{
public:
	Algorithm() {}
	virtual ~Algorithm() {}

	virtual std::deque<std::set<ChoiceID> > Run(const Choices &choices, const std::deque<Voter> &voters) = 0;
};

#endif
