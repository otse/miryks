#ifndef FILES_H
#define FILES_H

#include <string>
#include <vector>

int fbuf(const char *, char **, bool = false);

void fwrite(const std::string &, const std::string &);
std::string fread(const std::string &);
std::vector<unsigned char> *readbin(const std::string &);
bool exists_test3(const std::string &name);

#endif