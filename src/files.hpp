#ifndef FILES_HPP
#define FILES_HPP

#include <fstream>
#include <filesystem>

#include "idiom.hpp"

inline bool
fstat(const string &a)
{
	struct stat buffer;
	return (stat(a.c_str(), &buffer) == 0);
}

inline string
fread(const string &a)
{
	int mode = ifstream::binary | ifstream::ate;
	ifstream is(a, mode);
	if (is)
	{
		size_t end = is.tellg();
		string str(end, ' ');
		is.seekg(0, is.beg);
		is.read(&str[0], end);
		return str;
	}
	return "no";
}

inline vector<unsigned char>
freadbin(const string &a)
{
	int mode = ifstream::binary;
	ifstream b(a, mode);
	vector<unsigned char> c(
		istreambuf_iterator<char>(b), {});
	return c;
}

inline void
fwrite(const string &a, const string &b)
{
	ofstream os;
	os.open(a.c_str());
	os << b.c_str();
	os.close();
}

#endif