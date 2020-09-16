#include <dark2/files.h>

#include <fstream>
#include <iostream>
#include <filesystem>

#include <string>
#include <vector>
#include <map>

// written for filling out a plugin struct
int fbuf(const char *path, char **dest, bool cap)
{
	// printf("fbuf path %s", path);
	int mode = std::ifstream::binary | std::ifstream::ate;
	std::ifstream is(path, mode);
	if (is)
	{
		size_t end = is.tellg();
		*dest = (char *)malloc(end + (cap ? 1 : 0));
		is.seekg(0, is.beg);
		is.read((char *)*dest, end);
		if (cap)
		(*dest)[end] = '\0';
		return end;
	}
	return -1;
}

std::string fread(const std::string &a)
{
	int mode = std::ifstream::binary | std::ifstream::ate;
	std::ifstream is(a, mode);
	if (is)
	{
		size_t end = is.tellg();
		std::string str(end, ' ');
		is.seekg(0, is.beg);
		is.read(&str[0], end);
		return str;
	}
	return "no";
}

std::vector<unsigned char> *readbin(const std::string &a)
{
	std::ifstream input(a, std::ifstream::binary);
	std::vector<unsigned char> *c = new std::vector<unsigned char>(std::istreambuf_iterator<char>(input), {});
	return c;
}

void fwrite(const std::string &a, const std::string &b)
{
	std::ofstream os;
	os.open(a.c_str());
	os << b.c_str();
	os.close();
}

bool exists_test3(const std::string &name)
{
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}