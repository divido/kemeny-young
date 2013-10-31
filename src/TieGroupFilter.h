#ifndef KemenyYoung_TieGroupFilter
#define KemenyYoung_TieGroupFilter

#include "Algorithm.h"

class TieGroupFilter : public Algorithm
{
public:
	TieGroupFilter(Algorithm &backup);
	virtual ~TieGroupFilter();

	virtual std::deque<std::set<ChoiceID> > Run(const Choices &choices, const std::deque<Voter> &voters);

private:
	struct Internals;
	Internals *intern;
};

#endif
