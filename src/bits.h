typedef unsigned long long u_64;

inline u_64 shift_bit(int offset, int e)
{
      return 1ULL << (offset + e);
}

inline bool has_bit(u_64 flags, int e, int offset = 0)
{
      return (flags & shift_bit(offset, e)) == shift_bit(offset, e);
}

inline void set_bit(u_64 &flags, int e, int offset = 0)
{
      flags |= shift_bit(offset, e);
}