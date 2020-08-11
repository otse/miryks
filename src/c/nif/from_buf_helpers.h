#define nifn nif_t *nif, int n

#define hedr nif->hdr
#define buf nif->buf
#define pos nif->pos
#define blocks nif->blocks
#define byte_from_buf() *(unsigned char *)(buf + pos)
#define int_from_buf() *(int *)(buf + pos)
#define uint_from_buf() *(unsigned int*)(buf + pos)
#define float_from_buf() *(float *)(buf + pos)
#define short_from_buf() *(short *)(buf + pos)
#define ushort_from_buf() *(unsigned short *)(buf + pos)
#define vec_3_from_buf() *(vec_3 *)(buf + pos)
#define vec_4_from_buf() *(vec_4 *)(buf + pos)

#define one() pos += 1
#define two() pos += 2
#define four() pos += 4
#define eight() pos += 8