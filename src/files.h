#ifndef FILES_H
#define FILES_H

#include <fstream>
#include <filesystem>

#define NO_SUCH_THING "Inal bad, sorry"

#include "idiom.h"

#define CONCAT(a, b) a##b

#define NAME(y) CONCAT(tse, y)

//Dot fs::path argv0;

inline std::filesystem::path fpath(string2_t a)
{
	return std::filesystem::path(a);
}

inline bool fexists(string2_t a)
{
	return std::filesystem::exists(fpath(a));
}

inline string_t fstem(string2_t a)
{
	return fpath(a).stem().string();
}

inline std::vector<unsigned char> fread2(string2_t a)
{
	std::ifstream ifs(
		a, std::ifstream::binary | std::ios::ate);
	return std::vector<unsigned char>(
		std::istreambuf_iterator<char>(ifs), {});
}

inline string_t fread(string2_t a)
{
	std::ifstream ifs(
		a, std::ifstream::binary | std::ios::ate);
	if (!ifs)
		return NO_SUCH_THING;
	size_t end = ifs.tellg();
	std::string str(end, ' ');
	ifs.seekg(0, std::ios::beg);
	ifs.read(&str[0], end);
	return str;
}

inline void fwrite(string2_t a, string2_t b)
{
	std::ofstream ofs;
	ofs.open(a.c_str());
	ofs << b.c_str();
	ofs.close();
}

#endif