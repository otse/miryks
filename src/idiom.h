#ifndef IDIOM_H
#define IDIOM_H

#include <iostream>
#include <string>

using namespace std;

#define ERROR_FILE "error.txt"

template <typename... Args>
void log_(
    Args &&... args)
{
      (std::cout << ... << args) << std::endl;
}

#define assert_(e, m)                \
      if (!(e))                      \
      {                              \
            log_("assert hit: ", m); \
            fwrite(ERROR_FILE, m);   \
            exit(1);                 \
      }

#define exit_(m) ASSERT(false, m)

#endif