#pragma once

#include <string>
#include <map>
#include <vector>

extern "C"
{
#include <microtar.h>
}

// a resourcefile is a tar file

std::string resourcefile_offshore(
	const std::string &,
	const std::string &);
std::string resourcefile_offshore(
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
	std::vector<std::string> filenames;
	std::map<const std::string, const mtar_header_t> headers_by_name;
} Resourcefile;

extern std::map<std::string, Resourcefile *> resourcefiles;

Resourcefile &resourcefile_handle(std::string);
const mtar_header_t &resourcefile_get_header(const std::string &, std::string);
bool resourcefile_find(const std::string &, std::string);

std::string resourcefile_read(std::string, std::string);
std::string resourcefile_readgz(std::string, std::string);