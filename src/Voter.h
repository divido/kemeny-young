#ifndef KemenyYoung_Voter
#define KemenyYoung_Voter

#include "Choices.h"

class Voter
{
public:
	Voter(std::string name, const Choices &choices);
	Voter(const Voter &copy);
	virtual ~Voter();

	std::string name() const;

	void CastSelection(ChoiceID choice, unsigned preferenceLevel);

	// True if this voter prefers "preferred" over "comparison"
	bool Prefers(ChoiceID preferred, ChoiceID comparison) const;

	std::string AsString(std::string indent = "  ") const;

	Voter &operator = (const Voter &assign);

private:
	struct Internals;
	Internals *intern;
};

//--------------------------------------------------------------------------------

inline std::ostream &operator << (std::ostream &stream, const Voter &voter)
{
	return stream << voter.AsString();
}

#endif
