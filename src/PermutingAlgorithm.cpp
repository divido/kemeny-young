#include "PermutingAlgorithm.h"

#include <deque>
using std::deque;

#include <set>
using std::set;

#include <sstream>
using std::stringstream;

#include <iostream>
using std::cout;
using std::flush;
using std::endl;

#include <iomanip>
using std::setw;

#include <algorithm>
using std::next_permutation;

// --------------------------------------------------------------------------------

typedef deque<ChoiceID> Permutation;

// --------------------------------------------------------------------------------

struct PermutingAlgorithm::Internals
{
	void Print(const Choices &choices, const Permutation &permutation, unsigned score);
	deque<set<ChoiceID> > Merge(const deque<Permutation> &results);
};

PermutingAlgorithm::PermutingAlgorithm()
{
	intern = new Internals;
}

PermutingAlgorithm::~PermutingAlgorithm()
{
	delete intern;
}

deque<set<ChoiceID> > PermutingAlgorithm::Run(const Choices &choices, const deque<Voter> &voters)
{
	Permutation permutation;

	set<ChoiceID> allChoices = choices.allChoices();
	set<ChoiceID>::const_iterator choice;
	for (choice = allChoices.begin(); choice != allChoices.end(); choice++)
		permutation.push_back(*choice);

	deque<Permutation> results;
	unsigned highestScore = 0;

	unsigned numPermutations = 1;
	for (unsigned n = 0; n < permutation.size(); n++)
		numPermutations *= n + 1;

	unsigned numTried = 0;
	unsigned percentDone = 0;

	// Labels for the progress bar
	cout << "Permuting over " << permutation.size() << " entries (" << numPermutations << " permutations)" << endl;
	cout << "0%       10%       20%       30%       40%       50%       60%       70%       80%       90%      100%" << endl;

	// For each permutation, find all constraints within the permutation
	// (constraints take the form of "greater" > "lesser". Since the permutation
	// is sorted, this becomes easy. Next, find all voters that support that
	// constraint, add one to the score of the permutation for each such voter
	do
	{
		Permutation::const_iterator greater, lesser;
		deque<Voter>::const_iterator voter;
		unsigned score = 0;

		for (greater = permutation.begin(); greater != permutation.end(); greater++)
			for (lesser = greater + 1; lesser != permutation.end(); lesser++)
				for (voter = voters.begin(); voter != voters.end(); voter++)
					if (voter->Prefers(*greater, *lesser))
						score++;

		// Store a deque of tied-for-highest permutations
		if (score > highestScore)
		{
			highestScore = score;
			results.clear();
			results.push_back(permutation);
		}
		else if (score == highestScore)
		{
			results.push_back(permutation);
		}

		// Output the progress bar
		numTried++;
		while (numTried * 100 / numPermutations > percentDone && percentDone < 100)
		{
			cout << (percentDone % 10 == 0 ? "|" : ".") << flush;
			percentDone++;
		}
	} while (next_permutation(permutation.begin(), permutation.end()));

	// Finish out the progress bar
	while (percentDone <= 100)
	{
		cout << (percentDone % 10 == 0 ? "|" : ".") << flush;
		percentDone++;
	}
	cout << endl << endl;

	return intern->Merge(results);
}

void PermutingAlgorithm::Internals::Print(const Choices &choices, const Permutation &permutation, unsigned score)
{
	stringstream stream;
	stream << setw(3) << score << " ";

	bool first = true;
	Permutation::const_iterator value;
	for (value = permutation.begin(); value != permutation.end(); value++)
	{
		stream << (first ? "'" : ", '") << choices.LookupName(*value) << "'";
		first = false;
	}

	cout << stream.str() << endl;
}

deque<set<ChoiceID> > PermutingAlgorithm::Internals::Merge(const deque<Permutation> &results)
{
	deque<set<ChoiceID> > merged;

	unsigned length = results[0].size();
	unsigned index = 0;
	while (index < length)
	{
		set<ChoiceID> tiedResults;

		deque<Permutation>::const_iterator result;
		for (result = results.begin(); result != results.end(); result++)
		{
			const Permutation &permutation = *result;
			tiedResults.insert(permutation[index]);
		}

		merged.push_back(tiedResults);
		index += tiedResults.size();
	}

	return merged;
}
