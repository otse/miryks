#include <files.h>

#include <fstream>
#include <iostream>
#include <direct.h>
#include <filesystem>

#include <string>
#include <vector>

// https://stackoverflow.com/q/4426474/is-passing-pointer-argument-pass-by-value-in-c

// made to fill struct

char *get_text_file(const char *name) {
    char *x;
    fbuf(name, &x, true);
    return x;
}

char *get_binary_file(const char *name) {
	char *x;
	fbuf(name, &x, false);
	return x;
}

int fbuf(const char *path, char **dest, bool cap)
{
	// printf("fbuf path %s\n", path);
	int mode = std::ifstream::binary | std::ifstream::ate;
	std::ifstream is(path, mode);
	if (is)
	{
		unsigned int end = is.tellg();
		int len = end + (cap ? 1 : 0);
		*dest = (char *)malloc(len);
		is.seekg(0, is.beg);
		is.read((char *)*dest, end);
		if (cap)
			(*dest)[end] = '\0';
		return end;
	}
	return -1;
}

int fmkdir(const char *path)
{
	return mkdir(path);
}

std::string fread(const std::string &a)
{
	int mode = std::ifstream::binary | std::ifstream::ate;
	std::ifstream is(a, mode);
	if (is)
	{
		auto end = is.tellg();
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
	std::ofstream of;
	of.open(a.c_str());
	of << b.c_str();
	of.close();
}

bool exists_test3(const std::string &name)
{
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}