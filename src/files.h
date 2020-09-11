#ifndef FILES_H
#define FILES_H

#include "idiom.hpp"

int fbuf(const char *, char **, bool = false);

void fwrite(const string &, const string &);
string fread(const string &);
vector<unsigned char> *readbin(const string &);
bool exists_test3(const std::string &name);

#endif