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
	Algorithm(const Choices &choices, const std::deque<Voter> &voters);
	virtual ~Algorithm();

	void Run();
	std::string AsString(std::string indent = "  ") const;

protected:
	virtual std::deque<std::set<ChoiceID> > Execute(const std::set<ChoiceID> &choices, const std::deque<Voter> &voters) = 0;

private:
	struct Internals;
	Internals *intern;
};

// --------------------------------------------------------------------------------

inline std::ostream &operator << (std::ostream &stream, const Algorithm &algorithm)
{
	return stream << algorithm.AsString();
}

#endif
