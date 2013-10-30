#include "Algorithm.h"

#include <deque>
using std::deque;

#include <set>
using std::set;

#include <string>
using std::string;

#include <sstream>
using std::stringstream;

struct Algorithm::Internals
{
	const Choices *choices;
	const deque<Voter> *voters;

	deque<set<ChoiceID> > winner;
};

Algorithm::Algorithm(const Choices &choices, const std::deque<Voter> &voters)
{
	intern = new Internals;
	intern->choices = &choices;
	intern->voters = &voters;
}

Algorithm::~Algorithm()
{
	delete intern;
}

void Algorithm::Run()
{
	intern->winner = this->Execute(intern->choices->allChoices(), *intern->voters);
}

string Algorithm::AsString(string indent) const
{
	stringstream stream;

	deque<set<ChoiceID> >::const_iterator iter;
	for (iter = intern->winner.begin(); iter != intern->winner.end(); iter++)
	{
		bool first = true;
		set<ChoiceID>::const_iterator selection;
		for (selection = iter->begin(); selection != iter->end(); selection++)
		{
			stream << (first ? "'" : ", '") << intern->choices->LookupName(*selection) << "'";
			first = false;
		}
		stream << '\n';
	}

	return stream.str();
}
