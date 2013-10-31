#ifndef KemenyYoung_CondorcetFilter
#define KemenyYoung_CondorcetFilter

#include "Algorithm.h"

class CondorcetFilter : public Algorithm
{
public:
	CondorcetFilter(Algorithm &backup);
	virtual ~CondorcetFilter();

	virtual std::deque<std::set<ChoiceID> > Run(const Choices &choices, const std::deque<Voter> &voters);

private:
	struct Internals;
	Internals *intern;
};

#endif
