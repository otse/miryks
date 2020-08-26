#include <fstream>
#include <filesystem>

#include "files"

#include "idiom.hpp"

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

bool exists_test3(const std::string &name)
{
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}