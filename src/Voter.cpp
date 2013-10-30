#include "Voter.h"

#include <string>
using std::string;

#include <sstream>
using std::stringstream;

#include <deque>
using std::deque;

#include <set>
using std::set;

#include "StringFunctions.h"

struct Voter::Internals
{
	const Choices *choices;

	string name;
	deque<set<ChoiceID> > selections;
};

Voter::Voter(string name, const Choices &choices)
{
	intern = new Internals;
	intern->name = name;
	intern->choices = &choices;
}

Voter::Voter(const Voter &copy)
{
	intern = new Internals;
	*intern = *copy.intern;
}

Voter::~Voter()
{
	delete intern;
}

Voter &Voter::operator = (const Voter &assign)
{
	*intern = *assign.intern;
}

string Voter::name() const
{
	return intern->name;
}

void Voter::CastSelection(ChoiceID choice, unsigned preferenceLevel)
{
	while (intern->selections.size() <= preferenceLevel)
		intern->selections.push_back(set<ChoiceID>());

	intern->selections[preferenceLevel].insert(choice);
}

bool Voter::Prefers(ChoiceID preferred, ChoiceID comparison) const
{
	deque<set<ChoiceID> >::const_iterator iter;
	for (iter = intern->selections.begin(); iter != intern->selections.end(); iter++)
	{
		if (iter->count(comparison) > 0) return false;
		if (iter->count(preferred) > 0) return true;
	}

	return false;
}

string Voter::AsString(string indent) const
{
	stringstream stream;
	stream << indent << intern->name << ":\n";

	deque<set<ChoiceID> >::const_iterator iter;
	for (iter = intern->selections.begin(); iter != intern->selections.end(); iter++)
	{
		stream << indent << indent;

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
