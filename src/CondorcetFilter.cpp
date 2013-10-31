#include "CondorcetFilter.h"

#include "HeadOnHead.h"

#include <deque>
using std::deque;

#include <set>
using std::set;

#include <iostream>
using std::cout;
using std::flush;
using std::endl;

// --------------------------------------------------------------------------------

struct CondorcetFilter::Internals
{
	const Choices *choices;
	Algorithm *backup;
};

CondorcetFilter::CondorcetFilter(const Choices &choices, const deque<Voter> &voters, Algorithm &backup)
	: Algorithm(choices, voters)
{
	intern = new Internals;
	intern->choices = &choices;
	intern->backup = &backup;
}

CondorcetFilter::~CondorcetFilter()
{
	delete intern;
}

deque<set<ChoiceID> > CondorcetFilter::Execute(const set<ChoiceID> &choices, const deque<Voter> &voters)
{
	deque<ChoiceID> winners;
	deque<ChoiceID> losers;

	set<ChoiceID> contenders = choices;

	// Look for Condorcet winners / losers
	bool foundWinner, foundLoser;
	do
	{
		set<ChoiceID>::const_iterator candidate, opponent;
		for (candidate = contenders.begin(); candidate != contenders.end(); candidate++)
		{
			foundWinner = true;
			foundLoser = true;

			// Try all contenders. Usually, both foundWinner and foundLoser will be set to false
			for (opponent = contenders.begin(); opponent != contenders.end(); opponent++)
			{
				if (opponent == candidate) continue;

				int outcome = HeadOnHead(*candidate, *opponent, voters);
				if (outcome >= 0) foundLoser  = false;
				if (outcome <= 0) foundWinner = false;
			}

			if (foundWinner)
			{
				cout << intern->choices->LookupName(*candidate) << " is a condorcet winner" << endl;
				winners.push_back(*candidate);
				contenders.erase(candidate);
				break;
			}
			else if (foundLoser)
			{
				cout << intern->choices->LookupName(*candidate) << " is a condorcet loser" << endl;
				losers.push_back(*candidate);
				contenders.erase(candidate);
				break;
			}
		}
	} while (foundWinner || foundLoser);

	// ----------------------------------------

	deque<set<ChoiceID> > results;

	// Add the Condorcet Winners
	deque<ChoiceID>::iterator winner;
	for (winner = winners.begin(); winner != winners.end(); winner++)
	{
		set<ChoiceID> singleEntrySet;
		singleEntrySet.insert(*winner);
		results.push_back(singleEntrySet);
	}

	// Run the backup algorithm for the middle set
	intern->backup->RunConstrained(contenders);
	const deque<set<ChoiceID> > &backupResults = intern->backup->results();
	deque<set<ChoiceID> >::const_iterator backupResult;
	for (backupResult = backupResults.begin(); backupResult != backupResults.end(); backupResult++)
		results.push_back(*backupResult);

	// Add the Condorcet Losers
	deque<ChoiceID>::iterator loser;
	for (loser = losers.begin(); loser != losers.end(); loser++)
	{
		set<ChoiceID> singleEntrySet;
		singleEntrySet.insert(*loser);
		results.push_back(singleEntrySet);
	}

	return results;
}
