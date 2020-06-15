#ifndef IDIOM_H
#define IDIOM_H

#include <iostream>
#include <string>

#define ERROR_FILE "error.txt"

typedef const std::string &string2_t;

typedef std::string string_t;
typedef std::basic_string<unsigned char> ustring_t;

#define _string_ string_t
#define _string2_ string2_t

#define tse_that(this) auto &that = *this;

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