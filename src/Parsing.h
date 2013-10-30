#ifndef KemenyYoung_Parsing
#define KemenyYoung_Parsing

#include <istream>
#include <deque>

#include "Choices.h"
#include "Voter.h"

void Process(std::istream &stream, Choices &choices, std::deque<Voter> &voters);

#endif
