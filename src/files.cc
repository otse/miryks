#include <fstream>
#include <filesystem>

#include "files.h"

#include "idiom.hpp"

// written for filling out a plugin struct
int fbuf(const char *path, const char **dest)
{
	// printf("fbuf path %s", path);
	int mode = ifstream::binary | ifstream::ate;
	ifstream is(path, mode);
	if (is)
	{
		size_t end = is.tellg();
		*dest = (const char *)malloc(end);
		is.seekg(0, is.beg);
		is.read((char *)*dest, end);
		return end;
	}
	return -1;
}

string fread(const string &a)
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

vector<unsigned char> *readbin(const string &a)
{
	ifstream input(a, ifstream::binary);
	vector<unsigned char> *c = new vector<unsigned char>(istreambuf_iterator<char>(input), {});
	return c;
}

void fwrite(const string &a, const string &b)
{
	ofstream os;
	os.open(a.c_str());
	os << b.c_str();
	os.close();
}

bool exists_test3(const std::string &name)
{
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}