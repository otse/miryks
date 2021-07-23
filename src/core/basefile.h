#pragma once

#include <string>
#include <map>
#include <vector>

#include <microtar.h>

// depends on microtar

// a basefile is a tar file

std::string basefile_offshore(
	const std::string &,
	const std::string &);
std::string basefile_offshore(
	const std::string &,
	const std::string &,
	const std::string &,
	const std::string &);

int zinf(const void *, int, void *, int);
//static int zinf2(const void *, int, void *, int);

typedef struct
{
	std::string name;
	mtar_t mtar;
	std::vector<std::string> fnam;
	std::map<const std::string, const mtar_header_t> hnam;
} Basefile;

extern std::map<std::string, Basefile *> resourcefiles;

Basefile &basefile_handle(std::string);

bool basefile_find(const std::string &, std::string);

std::string basefile_read(std::string, std::string);
std::string basefile_readgz(std::string, std::string);