#ifndef KemenyYoung_StringFunctions
#define KemenyYoung_StringFunctions

#include <string>
#include <sstream>

template <class T>
inline std::string PrintIterable(const T &values)
{
	std::stringstream stream;

	bool first = true;
	typename T::const_iterator value;
	for (value = values.begin(); value != values.end(); value++)
	{
		stream << (first ? "'" : ", '") << *value << "'";
		first = false;
	}

	return stream.str();
}

inline std::string Trim(std::string str)
{
	size_t pos = str.find_first_not_of(" \t");
	if (pos == std::string::npos) return "";

	str = str.substr(pos);
	pos = str.find_last_not_of(" \t");
	return str.substr(0, pos + 1);
}

#endif
