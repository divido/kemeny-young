#ifndef KemenyYoung_CondorcetFilter
#define KemenyYoung_CondorcetFilter

#include "Algorithm.h"

class CondorcetFilter : public Algorithm
{
public:
	CondorcetFilter(const Choices &choices, const std::deque<Voter> &voters, Algorithm &backup);
	virtual ~CondorcetFilter();

protected:
	virtual std::deque<std::set<ChoiceID> > Execute(const std::set<ChoiceID> &choices, const std::deque<Voter> &voters);

private:
	struct Internals;
	Internals *intern;
};

#endif
