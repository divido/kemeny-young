#ifndef KemenyYoung_CandidateReductionFilter
#define KemenyYoung_CandidateReductionFilter

#include "Algorithm.h"

class CandidateReductionFilter : public Algorithm
{
public:
	CandidateReductionFilter(size_t limit, Algorithm &backup);
	virtual ~CandidateReductionFilter();

	virtual std::deque<std::set<ChoiceID> > Run(const Choices &choices, const std::deque<Voter> &voters);

private:
	struct Internals;
	Internals *intern;
};

#endif
