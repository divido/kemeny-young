#include "HeadOnHead.h"

#include <deque>
using std::deque;

int HeadOnHead(ChoiceID candidate, ChoiceID opponent, const deque<Voter> &voters)
{
	deque<Voter>::const_iterator voter;

	int result = 0;

	for (voter = voters.begin(); voter != voters.end(); voter++)
	{
		if (voter->Prefers(candidate, opponent)) result++;
		if (voter->Prefers(opponent, candidate)) result--;
	}

	return result;
}
