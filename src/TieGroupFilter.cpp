#include "TieGroupFilter.h"

#include "HeadOnHead.h"

#include <deque>
using std::deque;

#include <set>
using std::set;

#include <map>
using std::map;

#include <sstream>
using std::stringstream;

#include <iostream>
using std::cout;
using std::flush;
using std::endl;

// --------------------------------------------------------------------------------

struct TieGroupFilter::Internals
{
	Internals(Algorithm &setBackup) : backup(setBackup) {}

	Algorithm &backup;
};

TieGroupFilter::TieGroupFilter(Algorithm &backup)
{
	intern = new Internals(backup);
}

TieGroupFilter::~TieGroupFilter()
{
	delete intern;
}

bool BelongsInGroup(ChoiceID candidate, ChoiceID leader, const set<ChoiceID> &members, const deque<Voter> &voters, const Choices &choices)
{
	// Check opponent against the group leader
	int outcome = HeadOnHead(candidate, leader, voters);
	if (outcome == 0) return true;

	// Check opponent against all group members
	set<ChoiceID>::const_iterator member;
	for (member = members.begin(); member != members.end(); member++)
	{
		int outcome = HeadOnHead(candidate, *member, voters);
		if (outcome == 0) return true;
	}
}

deque<set<ChoiceID> > TieGroupFilter::Run(const Choices &choices, const deque<Voter> &voters)
{
	map<ChoiceID, set<ChoiceID> > groups;
	set<ChoiceID> individuals = choices.allChoices();
	Choices groupChoices = choices;

	bool formedGroup;
	do
	{
		formedGroup = false;

		set<ChoiceID>::const_iterator leader;
		for (leader = individuals.begin(); leader != individuals.end(); leader++)
		{
			set<ChoiceID> group;

			// Try all individuals, looking for those that tie with candidate
			// We need to do this repeatedly until there are no longer any ties,
			// since some members only join the group once others join.

			bool addedMember;
			do
			{
				addedMember = false;

				set<ChoiceID>::const_iterator candidate;
				for (candidate = individuals.begin(); candidate != individuals.end(); candidate++)
				{
					if (leader == candidate) continue;
					if (group.count(*candidate) > 0) continue;

					if (BelongsInGroup(*candidate, *leader, group, voters, choices))
					{
						group.insert(*candidate);
						addedMember = true;
					}
				}
			} while (addedMember);

			if (group.size() > 0)
			{
				// Remove all group members from the individuals set
				set<ChoiceID>::const_iterator member;
				for (member = group.begin(); member != group.end(); member++)
				{
					individuals.erase(*member);
					groupChoices.RemoveChoice(*member);
				}

				// Remove the leader from the individuals set, too, so we stop
				// considering him for group membership. However, he remains in
				// groupChoices to represent the group.
				individuals.erase(*leader);

				// Remember the group that he's leading
				groups[*leader] = group;

				// Flag that we should keep going, then break out of the for
				// loop (iterators will have been invalidated by the erase)
				formedGroup = true;
				break;
			}
		}
	} while (formedGroup);

	// ----------------------------------------

	cout << "Groups:" << endl;
	map<ChoiceID, set<ChoiceID> >::const_iterator group;
	for (group = groups.begin(); group != groups.end(); group++)
	{
		cout << "'" << choices.LookupName(group->first) << "' is standing in for: ";
		bool first = true;
		set<ChoiceID>::const_iterator member;
		for (member = group->second.begin(); member != group->second.end(); member++)
		{
			cout << (first ? "" : ", ") << "'" << choices.LookupName(*member) << "'";
			first = false;
		}
		cout << endl;
	}
	cout << endl;

	// ----------------------------------------

	deque<set<ChoiceID> > results = intern->backup.Run(groupChoices, voters);

	deque<set<ChoiceID> >::iterator result;
	for (result = results.begin(); result != results.end(); result++)
	{
		for (group = groups.begin(); group != groups.end(); group++)
		{
			ChoiceID leader = group->first;
			if (result->count(leader))
				result->insert(group->second.begin(), group->second.end());
		}
	}

	return results;
}
