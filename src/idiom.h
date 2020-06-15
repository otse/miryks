#ifndef IDIOM_H
#define IDIOM_H

#include <iostream>
#include <string>

using namespace std;

#define ERROR_FILE "error.txt"

template <typename... Args>
void MAGIC(
    Args &&... args)
{
      (std::cout << ... << args) << std::endl;
}

#define ASSERT(e, m)                  \
      if (e == false)                 \
      {                               \
            MAGIC("assert hit: ", m); \
            fwrite(ERROR_FILE, m);    \
            exit(1);                  \
      }

#define EXIT(m) ASSERT(false, m)

#endif