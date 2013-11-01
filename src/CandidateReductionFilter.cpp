#include "CandidateReductionFilter.h"

#include <deque>
using std::deque;

#include <set>
using std::set;

#include <map>
using std::multimap;
using std::make_pair;
using std::pair;

#include <iostream>
using std::cout;
using std::flush;
using std::endl;

// --------------------------------------------------------------------------------

struct CandidateReductionFilter::Internals
{
	Internals(Algorithm &setBackup) : backup(setBackup) {}

	size_t limit;
	Algorithm &backup;

	unsigned ScoreChoice(ChoiceID choice, const set<ChoiceID> &choices, const deque<Voter> &voters);
};

CandidateReductionFilter::CandidateReductionFilter(size_t limit, Algorithm &backup)
{
	intern = new Internals(backup);
	intern->limit = limit;
}

CandidateReductionFilter::~CandidateReductionFilter()
{
	delete intern;
}

deque<set<ChoiceID> > CandidateReductionFilter::Run(const Choices &choices, const deque<Voter> &voters)
{
	typedef multimap<unsigned, ChoiceID> ScoreMap;

	ScoreMap scored;
	Choices constrained = choices;

	set<ChoiceID> allChoices = choices.allChoices();
	set<ChoiceID>::const_iterator choice;
	for (choice = allChoices.begin(); choice != allChoices.end(); choice++)
	{
		unsigned score = intern->ScoreChoice(*choice, allChoices, voters);
		scored.insert(make_pair(score, *choice));
	}

	// ----------------------------------------

	while (scored.size() > intern->limit)
	{
		pair<ScoreMap::iterator, ScoreMap::iterator> deleteRange;
		deleteRange = scored.equal_range(scored.begin()->first);

		ScoreMap::iterator entry;
		for (entry = deleteRange.first; entry != deleteRange.second; entry++)
			constrained.RemoveChoice(entry->second);

		scored.erase(deleteRange.first, deleteRange.second);
	}

	// ----------------------------------------

	cout << "Constrained choices to " << scored.size() << endl;
	cout << "New Choices:" << endl;
	cout << constrained << endl << endl;

	return intern->backup.Run(constrained, voters);
}

unsigned CandidateReductionFilter::Internals::ScoreChoice(ChoiceID choice, const set<ChoiceID> &choices, const deque<Voter> &voters)
{
	unsigned score = 0;

	deque<Voter>::const_iterator voter;
	for (voter = voters.begin(); voter != voters.end(); voter++)
	{
		set<ChoiceID>::const_iterator test;
		for (test = choices.begin(); test != choices.end(); test++)
			if (voter->Prefers(choice, *test)) score++;
	}

	return score;
}
