#include <iostream>
using std::ostream;
using std::cout;
using std::cerr;
using std::endl;

#include <fstream>
using std::fstream;
using std::ios;

#include <sstream>
using std::stringstream;

#include <string>
using std::string;

#include <set>
using std::set;

#include <deque>
using std::deque;

#include <map>
using std::map;

#include <algorithm>
using std::next_permutation;
using std::sort;
using std::reverse;

// --------------------------------------------------------------------------------

struct Voter
{
	string name;
	deque<set<string> > votes;
};

struct Result
{
	deque<string> order;
	unsigned score;

	bool operator < (const Result &other) const { return score < other.score; }
};

// ----------------------------------------

class KemenyYoungVoting
{
public:
	KemenyYoungVoting(string filename);

	void Print(ostream &stream);

private:
	set<string> choices;
	deque<Voter> voters;
	deque<Result> results;

	void ProcessFile(string filename);
	void ComputeScores();
};


// --------------------------------------------------------------------------------

template <class T>
inline string PrintIterable(const T &values)
{
	stringstream stream;

	bool first = true;
	typename T::const_iterator value;
	for (value = values.begin(); value != values.end(); value++)
	{
		stream << (first ? "'" : ", '") << *value << "'";
		first = false;
	}

	return stream.str();
}

inline string Trim(string str)
{
	size_t pos = str.find_first_not_of(" \t");
	if (pos == string::npos) return "";

	str = str.substr(pos);
	pos = str.find_last_not_of(" \t");
	return str.substr(0, pos + 1);
}

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

	KemenyYoungVoting results(argv[1]);
	results.Print(cout);
	cout << endl;

	return 0;
}

// --------------------------------------------------------------------------------

KemenyYoungVoting::KemenyYoungVoting(string filename)
{
	ProcessFile(filename);
	ComputeScores();
}

// --------------------------------------------------------------------------------

void KemenyYoungVoting::ProcessFile(string filename)
{
	fstream file(filename.c_str(), ios::in);
	string line;

	while (getline(file, line))
	{
		size_t colon = line.find(":");
		if (colon == string::npos) continue;

		Voter voter;
		voter.name = Trim(line.substr(0, colon));

		bool equalPreference = false;
		size_t sep;
		string remaining = line.substr(colon + 1);

		while (true)
		{
			sep = remaining.find_first_of(">=");
			string vote = Trim(remaining.substr(0, sep));
			choices.insert(vote);

			if (! equalPreference) voter.votes.push_back(set<string>());
			voter.votes.back().insert(vote);

			if (sep == string::npos) break;

			equalPreference = (bool)(remaining[sep] == '=');
			remaining = remaining.substr(sep + 1);
		}

		voters.push_back(voter);
	}
}

// --------------------------------------------------------------------------------

void KemenyYoungVoting::ComputeScores()
{
	deque<string> permutation;
	set<string>::const_iterator choice;
	for (choice = choices.begin(); choice != choices.end(); choice++)
		permutation.push_back(*choice);

	// For each permutation, find all constraints within the permutation
	// (constraints take the form of "greater" > "lesser". Since the permutation
	// is sorted, this becomes easy. Next, find all voters that support that
	// constraint, add one to the score of the permutation for each such voter
	do
	{
		Result result;
		result.order = permutation;
		result.score = 0;

		deque<string>::const_iterator greater, lesser;
		for (greater = permutation.begin(); greater != permutation.end(); greater++)
		{
			for (lesser = greater + 1; lesser != permutation.end(); lesser++)
			{
				deque<Voter>::const_iterator voter;
				for (voter = voters.begin(); voter != voters.end(); voter++)
				{
					// To determine if the voter contains that constraint, look
					// through the sorted list of "equal preference" groups
					// (each group is preferred less than the previous group,
					// but all members of the group are preferred equally). If
					// a group containing "lesser" is found first, then the
					// constraint isn't held. Otherwise, if "greater" is found,
					// then it is. Note: If both are found, the constraint isn't held.
					deque<set<string> >::const_iterator vote;
					for (vote = voter->votes.begin(); vote != voter->votes.end(); vote++)
					{
						bool containsGreater = (bool)(vote->count(*greater) > 0);
						bool containsLesser  = (bool)(vote->count(*lesser)  > 0);

						if (containsLesser) break;
						if (containsGreater)
						{
							result.score++;
							break;
						}
					}
				}
			}
		}

		results.push_back(result);
	} while (next_permutation(permutation.begin(), permutation.end()));

	// Sort by score, then reverse to get the highest score at the front
	sort(results.begin(), results.end());
	reverse(results.begin(), results.end());
}

// --------------------------------------------------------------------------------

void KemenyYoungVoting::Print(ostream &stream)
{
	stream << choices.size() << " Choices: " <<	PrintIterable(choices);

	stream << "\n" << voters.size() << " Voters:\n";
	deque<Voter>::const_iterator voter;
	for (voter = voters.begin(); voter != voters.end(); voter++)
	{
		stream << "  Name: '" << voter->name << "'\n";

		unsigned i = 0;
		for (i = 0; i < voter->votes.size(); i++)
			stream << "  [" << i << "] = " << PrintIterable(voter->votes[i]) << '\n';
	}

	if (results.size() < 100)
	{
		stream << "\nResults:\n";
		deque<Result>::const_iterator result;
		for (result = results.begin(); result != results.end(); result++)
			stream << PrintIterable(result->order) << " = " << result->score << '\n';
	}

	if (results.size() > 0)
	{
		unsigned topScore = results.front().score;
		set<string> topChoice;

		deque<Result>::iterator result;
		for (result = results.begin(); result != results.end(); result++)
		{
			if (result->score == topScore)
				topChoice.insert(result->order.front());
		}

		stream << "\nTop Choice:\n" << PrintIterable(topChoice);

		if (topChoice.size() == 1) stream << " -- Outright\n";
		else stream << " -- Tied\n";
	}
}
