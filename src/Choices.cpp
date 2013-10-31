#include "Choices.h"

#include <map>
using std::map;

#include <set>
using std::set;

#include <string>
using std::string;

#include <sstream>
using std::stringstream;

#include <stdexcept>
using std::runtime_error;

struct Choices::Internals
{
	map<string, ChoiceID> data;
	ChoiceID nextID;
};

Choices::Choices()
{
	intern = new Internals;
	intern->nextID = 0;
}

Choices::Choices(const Choices &copy)
{
	intern = new Internals;
	*intern = *copy.intern;
}

Choices::~Choices()
{
	delete intern;
}

Choices &Choices::operator = (const Choices &assign)
{
	*intern = *assign.intern;
}

set<ChoiceID> Choices::allChoices() const
{
	set<ChoiceID> all;

	map<string, ChoiceID>::const_iterator entry;
	for (entry = intern->data.begin(); entry != intern->data.end(); entry++)
		all.insert(entry->second);

	return all;
}

void Choices::AddChoice(string choice)
{
	if (intern->data.count(choice) == 0)
		intern->data[choice] = intern->nextID++;
}

void Choices::RemoveChoice(ChoiceID id)
{
	map<string, ChoiceID>::iterator entry;

	for (entry = intern->data.begin(); entry != intern->data.end(); entry++)
	{
		if (entry->second == id)
		{
			intern->data.erase(entry);
			return;
		}
	}

	throw runtime_error("RemoveChoice called for bad id");
}

ChoiceID Choices::LookupID(string choice) const
{
	map<string, ChoiceID>::iterator entry = intern->data.find(choice);
	if (entry == intern->data.end()) throw runtime_error("LookupID called for bad choice");

	return entry->second;
}

string Choices::LookupName(ChoiceID id) const
{
	map<string, ChoiceID>::iterator entry;

	for (entry = intern->data.begin(); entry != intern->data.end(); entry++)
		if (entry->second == id) return entry->first;

	throw runtime_error("LookupName called for bad id");
}

string Choices::AsString(string indent) const
{
	stringstream stream;

	map<string, ChoiceID>::const_iterator entry;
	for (entry = intern->data.begin(); entry != intern->data.end(); entry++)
		stream << indent << "'" << entry->first << "'\n";

	return stream.str();
}

