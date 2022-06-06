#pragma once

// part of gloom

#include <string>
#include <vector>

int fbuf(const char *, char **, bool = true);
int fmkdir(const char *);

char *get_text_file(const char *);
char *get_binary_file(const char *);

void fwrite(const std::string &, const std::string &);
std::string fread(const std::string &);
std::vector<unsigned char> *readbin(const std::string &);
bool exists_test3(const std::string &name);