#include "Parsing.h"
#include "Choices.h"
#include "Voter.h"

#include "PermutingAlgorithm.h"
#include "CondorcetFilter.h"
#include "TieGroupFilter.h"
#include "CandidateReductionFilter.h"

#include <fstream>
using std::fstream;

#include <deque>
using std::deque;

#include <set>
using std::set;

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

// --------------------------------------------------------------------------------

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		cerr << "Usage: " << argv[0] << " VOTE_FILE\n";
		cerr << "The VOTE_FILE should contain lines of the format:\n";
		cerr << "   VoterName: Choice > Choice = Choice > Choice\n";
		cerr << "\n'VoterName' and 'Choice' cannot contain :, >, or =\n";
		cerr << "Choices should be separated by > to express preference or = to express no preference" << endl;
		return 0;
	}

	// ----------------------------------------

	Choices choices;
	deque<Voter> voters;

	fstream input(argv[1]);
	Process(input, choices, voters);

	cout << "Choices:" << endl << choices << endl;
	cout << "Voters:" << endl;
	deque<Voter>::const_iterator voter;
	for (voter = voters.begin(); voter != voters.end(); voter++)
		cout << *voter << endl;

	// ----------------------------------------

	PermutingAlgorithm permuting;
	CondorcetFilter condorcet(permuting);
	CandidateReductionFilter reduction(10, condorcet);

	Algorithm &algorithm = reduction;
	deque<set<ChoiceID> > winner = algorithm.Run(choices, voters);

	// ----------------------------------------

	cout << "Results:" << endl;

	deque<set<ChoiceID> >::const_iterator iter;
	for (iter = winner.begin(); iter != winner.end(); iter++)
	{
		bool first = true;
		set<ChoiceID>::const_iterator selection;
		for (selection = iter->begin(); selection != iter->end(); selection++)
		{
			cout << (first ? "'" : ", '") << choices.LookupName(*selection) << "'";
			first = false;
		}
		cout << endl;
	}

	cout << endl;

	return 0;
}
