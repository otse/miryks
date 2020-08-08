#include "c/c.h"

#include "nif.h"

nmap_t nmap[1000];
int nifs = 0;

//const char *path = "path not set";

api nif_t *nif_alloc() {
	nif_t *bucket = malloc(sizeof(nif_t));
	bucket->pos = 0;
	bucket->buf = 0;
	return bucket;
}

#define hedr nif->hdr
#define buf nif->buf
#define pos nif->pos

#define one() pos += 1;
#define two() pos += 2;
#define four() pos += 4;
#define eight() pos += 8;

char *nif_read_short_string(nif_t *nif) {
	uint8_t len = *(buf + pos);
	char *string = malloc(len * sizeof(char));
	strncpy(string, buf + 1 + pos, len);
	pos += len + 1;
	return string;
}

void nif_read_header(nif_t *nif) {
#define deref() *(buf + pos)
	nif_read_header_string(nif);
	hedr.unknown_1 = deref();
	four();
	//cassert_(pos == 0x2b, "0x2b");
	nif_read_header_tween(nif);
	nif_read_block_types(nif);
}

void nif_read_header_tween(nif_t *nif) {
	hedr.endian_type = deref();
	one();
	hedr.user_value = deref();
	four();
	hedr.num_blocks = deref();
	four();
	hedr.user_value_2 = deref();
	four();
	hedr.author = nif_read_short_string(nif, &pos);
	hedr.process_script = nif_read_short_string(nif);
	hedr.export_script = nif_read_short_string(nif);
	hedr.num_block_types = deref();
	two();
}

void nif_read_header_string(nif_t *nif) {
	// e.g. Gamebryo File Format, Version 20.2.0.7\n
	int n = strchr(buf, '\n') - buf + 1;
	char *string = malloc(n * sizeof(char));
	strncpy(string, buf, n);
	string[n - 1] = '\0';
	hedr.header_string = string;
	hedr.version = string + 0x1e;
	pos += n;
}

void nif_read_block_types(nif_t *nif) {

}

api void nif_add(void *key, nif_t *nif)
{
	nif->n = nifs;
	nmap[nifs++] = (nmap_t){key, nif};
}

api nif_t *nif_get_stored(void *key)
{
	for (int i = 0; i < nifs; i++)
	{
	if (nmap[i].key == key)
	{
	return nmap[i].value;
	}
	}
	return 0;
}

api void nif_make(void *key, nif_t *nif)
{
	cassert_(buf, "pls set nif->buf");

	//memcpy(&nif->hdr, nif->buf, sizeof(nif_hedr_t));
	nif_read_header(nif);

	printf("memcpy %s\n", hedr.header_string);

	nif_add(key, nif);
}
