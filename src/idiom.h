#pragma once

#include <iostream>
#include <string>

// Ojeng Tsunomi aka Tse Ojeng

typedef void nil;

// #define _ void

typedef const std::string &string2_t;

typedef std::string string_t;
typedef std::basic_string<unsigned char> ustring_t;

#ifdef NDEBUG
const bool tsedebug = false;
#elif DEBUG
const bool tsedebug = true;
#endif

#define tse_that(this) auto &that = *this;

template <typename... Args>
void tse_log(
    Args &&... args)
{
      (std::cout << ... << args) << std::endl;
}

template <typename... Args>
string_t tse_cat(
    Args &&... args)
{
      return (... + args);
}

#define tse_assert(e, m)              \
      if (!(e))                       \
      {                               \
            tse_log("exiting: ", m);  \
            fwrite("error.txt", m); \
            exit(1);                  \
      }

#define tse_exit(m) tse_assert(false, m)

typedef unsigned long long u_64;

inline u_64 TSE_SHIFT(int offset, int e)
{
      return 1ULL << (offset + e);
}

inline bool TSE_HAS_BIT(u_64 flags, int e, int offset = 0)
{
      return (flags & TSE_SHIFT(offset, e)) == TSE_SHIFT(offset, e);
}

inline void TSE_SET_BIT(u_64 &flags, int e, int offset = 0)
{
      flags |= TSE_SHIFT(offset, e);
}