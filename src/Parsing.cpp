#include "Parsing.h"

#include <istream>
using std::istream;

#include <deque>
using std::deque;

#include <string>
using std::string;

#include "StringFunctions.h"

void Process(istream &stream, Choices &choices, deque<Voter> &voters)
{
	string line;

	while (getline(stream, line))
	{
		// Preprocess comments
		size_t comment = line.find("#");
		if (comment != string::npos)
			line = line.substr(0, comment);

		size_t colon = line.find(":");
		if (colon == string::npos) continue;

		Voter voter(Trim(line.substr(0, colon)), choices);

		unsigned preferenceLevel = 0;
		string remaining = line.substr(colon + 1);
		size_t sep;

		while (true)
		{
			sep = remaining.find_first_of(">=");
			string selection = Trim(remaining.substr(0, sep));

			choices.AddChoice(selection);
			voter.CastSelection(choices.LookupID(selection), preferenceLevel);

			if (sep == string::npos) break;
			if (remaining[sep] == '>') preferenceLevel++;
			remaining = remaining.substr(sep + 1);
		}

		voters.push_back(voter);
	}
}
