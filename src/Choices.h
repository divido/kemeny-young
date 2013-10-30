#ifndef KemenyYoung_Choices
#define KemenyYoung_Choices

#include <string>
#include <ostream>
#include <set>

typedef unsigned ChoiceID;

// --------------------------------------------------------------------------------

class Choices
{
public:
	Choices();
	Choices(const Choices &copy);
	virtual ~Choices();

	std::set<ChoiceID> allChoices() const;
	void AddChoice(std::string choice);

	ChoiceID LookupID(std::string choice) const;
	std::string LookupName(ChoiceID id) const;

	std::string AsString(std::string indent = "  ") const;

	Choices &operator = (const Choices &assign);

private:
	struct Internals;
	Internals *intern;
};

// --------------------------------------------------------------------------------

inline std::ostream &operator << (std::ostream &stream, const Choices &choices)
{
	return stream << choices.AsString();
}

#endif
