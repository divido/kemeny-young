#include "Parsing.h"
#include "Choices.h"
#include "Voter.h"
#include "PermutingAlgorithm.h"

#include <fstream>
using std::fstream;

#include <deque>
using std::deque;

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

	Choices choices;
	deque<Voter> voters;

	fstream input(argv[1]);
	Process(input, choices, voters);

	cout << "Choices:" << endl << choices << endl;
	cout << "Voters:" << endl;
	deque<Voter>::const_iterator voter;
	for (voter = voters.begin(); voter != voters.end(); voter++)
		cout << *voter << endl;

	PermutingAlgorithm algorithm(choices, voters);
	algorithm.Run();
	cout << "Results:" << endl << algorithm << endl;

	return 0;
}
