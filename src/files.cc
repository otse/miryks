#include <fstream>
#include <filesystem>

#include "files"

#include "idiom.hpp"

char *fstem(const char *p, char delim)
{
	const char *c = strrchr(p, delim);
	int l = c - p + 1;
	char *d = (char *)malloc(l);
	memcpy(d, p, l-1);
	d[l-1] = '\0';
	return d;
}

char *fname(const char *p, char delim)
{
	const char *c = strrchr(p, delim) + 1;
	int l = strlen(p) - (c - p);
	char *d = (char *)malloc(l + 1);
	memcpy(d, c, l);
	d[l] = '\0';
	return d;
}

bool fstat(const string &a)
{
	struct stat buffer;
	return (stat(a.c_str(), &buffer) == 0);
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

vector<unsigned char> freadbin(const string &a)
{
	int mode = ifstream::binary;
	ifstream b(a, mode);
	vector<unsigned char> c(
		istreambuf_iterator<char>(b), {});
	return c;
}

void fwrite(const string &a, const string &b)
{
	ofstream os;
	os.open(a.c_str());
	os << b.c_str();
	os.close();
}