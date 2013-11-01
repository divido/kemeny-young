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

#include <stdexcept>
using std::logic_error;

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
	if (allChoices.size() == 0)
		return deque<set<ChoiceID> >();

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

	// Print out the results
	deque<Permutation>::const_iterator result;
	for (result = results.begin(); result != results.end(); result++)
	{
		cout << highestScore << " ";

		bool first = true;
		Permutation::const_iterator permutation;
		for (permutation = result->begin(); permutation != result->end(); permutation++)
		{
			cout << (first ? "" : ", ") << "'" << choices.LookupName(*permutation) << "'";
			first = false;
		}

		cout << endl;
	}

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

inline int CompareInPermutations(ChoiceID first, ChoiceID second, const deque<Permutation> &results)
{
	int expectation = 0;

	deque<Permutation>::const_iterator result;
	for (result = results.begin(); result != results.end(); result++)
	{
		int comparison = 0;
		deque<ChoiceID>::const_iterator iter;

		for (iter = result->begin(); iter != result->end(); iter++)
		{
			if (*iter == first)  { comparison = -1; break; }
			if (*iter == second) { comparison =  1; break; }
		}

		if (comparison == 0)
			throw logic_error("Choices aren't present in result set?!");

		// First time through. Set up expectation
		if (expectation == 0) expectation = comparison;

		// Otherwise, if we don't match the expectation, then the choices arrive
		// in different orders in different permutations. Consider them "equal"
		else if (comparison != expectation) return 0;
	}

	if (expectation == 0)
		throw logic_error("Result set seems to be empty, can't compare");

	return expectation;
}

int CompareInPermutations(ChoiceID first, const set<ChoiceID> &seconds, const deque<Permutation> &results)
{
	bool firstCompare = true;
	int comparison;

	set<ChoiceID>::const_iterator second;
	for (second = seconds.begin(); second != seconds.end(); second++)
	{
		int thisComparison = CompareInPermutations(first, *second, results);
		if (firstCompare) comparison = thisComparison;
		else if (comparison != thisComparison) return 0;
		firstCompare = false;
	}

	if (firstCompare) throw logic_error("Empty seconds set, Can't compare");
	return comparison;
}

int CompareInPermutations(const set<ChoiceID> &firsts, const set<ChoiceID> &seconds, const deque<Permutation> &results)
{
	bool firstCompare = true;
	int comparison;

	set<ChoiceID>::const_iterator first;
	for (first = firsts.begin(); first != firsts.end(); first++)
	{
		int thisComparison = CompareInPermutations(*first, seconds, results);
		if (firstCompare) comparison = thisComparison;
		else if (comparison != thisComparison) return 0;
		firstCompare = false;
	}

	if (firstCompare) throw logic_error("Empty firsts set, Can't compare");
	return comparison;
}

deque<set<ChoiceID> > PermutingAlgorithm::Internals::Merge(const deque<Permutation> &results)
{
	deque<set<ChoiceID> > merged;
	if (results.size() == 0) return merged;

	// Populate the sets with a default single entry set
	Permutation::const_iterator choice;
	for (choice = results[0].begin(); choice != results[0].end(); choice++)
	{
		set<ChoiceID> singleEntrySet;
		singleEntrySet.insert(*choice);
		merged.push_back(singleEntrySet);
	}

	bool changedSomething;
	do
	{
		changedSomething = false;

		for (unsigned i = 0; i < merged.size() - 1; i++)
		{
			set<ChoiceID> &first = merged[i];
			set<ChoiceID> &second = merged[i + 1];

			int comparison = CompareInPermutations(first, second, results);

			if (comparison > 0)
			{
				// Order is reversed
				first.swap(second);
				changedSomething = true;
			}
			else if (comparison == 0)
			{
				// Equal, therefore we must merge them
				first.insert(second.begin(), second.end());
				merged.erase(merged.begin() + i + 1);
				changedSomething = true;
			}
		}
	} while (changedSomething);

	return merged;
}
