#define nifn nif_t *nif, int n

#define hedr nif->hdr
#define buf nif->buf
#define pos nif->pos
#define blocks nif->blocks
#define from_buf() *(buf + pos)

#define one() pos += 1
#define two() pos += 2
#define four() pos += 4
#define eight() pos += 8