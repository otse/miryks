#ifndef FILES_H
#define FILES_H

// part of gloom

#include <string>
#include <vector>

int fbuf(const char *, char **, bool = false);
int fmkdir(const char *);

void fwrite(const std::string &, const std::string &);
std::string fread(const std::string &);
std::vector<unsigned char> *readbin(const std::string &);
bool exists_test3(const std::string &name);

#endif