#ifndef FILES_H
#define FILES_H

#include <fstream>
#include <filesystem>

#include "idiom.h"

inline bool
fstat(_string2_ a)
{
	struct stat buffer;
	return (stat(a.c_str(), &buffer) == 0);
}

inline _string_
fread(_string2_ a)
{
	using namespace std;

	int mode = ifstream::binary | ifstream::ate;
	ifstream is(a, mode);
	if (is)
	{
		size_t end = is.tellg();
		_string_ str(end, ' ');
		is.seekg(0, is.beg);
		is.read(&str[0], end);
		return str;
	}
}

inline std::vector<unsigned char>
freadbin(_string2_ a)
{
	using namespace std;
	int mode = ifstream::binary;
	ifstream b(a, mode);
	std::vector<unsigned char> c(
		std::istreambuf_iterator<char>(b), {});
	return c;
}

inline void
fwrite(_string2_ a, _string2_ b)
{
	using namespace std;
	ofstream os;
	os.open(a.c_str());
	os << b.c_str();
	os.close();
}

#endif