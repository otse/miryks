#include "c/c.h"

#include "nif.h"

#define hedr nif->hdr
#define buf nif->buf
#define pos nif->pos
#define from_buf() *(buf + pos);

#define one() pos += 1;
#define two() pos += 2;
#define four() pos += 4;
#define eight() pos += 8;

char *nif_read_short_string(nif_t *nif)
{
	char len = *(buf + pos);
	char *string = malloc(len * sizeof(char));
	strncpy(string, buf + 1 + pos, len);
	pos += len + 1;
	return string;
}

char *nif_read_sized_string(nif_t *nif)
{
	int len = *(buf + pos);
	char *string = malloc(len + 1 * sizeof(char));
	strncpy(string, buf + pos + 4, len);
	string[len] = '\0';
	four();
	pos += len;
	return string;
}

void nif_read_header(nif_t *nif)
{
	nif_read_header_string(nif);
	hedr.unknown_1 = from_buf();
	four();
	nif_read_some_stuff(nif);
	nif_read_block_types(nif);
	nif_read_block_type_index(nif);
	nif_read_block_sizes(nif);
	nif_read_strings(nif);
	nif_read_groups(nif);
	hedr.end = pos;
}

void nif_read_header_string(nif_t *nif)
{
	// Gamebryo File Format, Version 20.2.0.7\n
	int n = strchr(buf, '\n') - buf + 1;
	char *string = malloc(n * sizeof(char));
	strncpy(string, buf, n);
	string[n - 1] = '\0';
	hedr.header_string = string;
	hedr.version = string + 30;
	pos += n;
}

void nif_read_some_stuff(nif_t *nif)
{
	hedr.endian_type = from_buf();
	one();
	hedr.user_value = from_buf();
	four();
	hedr.num_blocks = from_buf();
	four();
	hedr.user_value_2 = from_buf();
	four();
	hedr.author = nif_read_short_string(nif);
	hedr.process_script = nif_read_short_string(nif);
	hedr.export_script = nif_read_short_string(nif);
	hedr.num_block_types = from_buf();
	two();
}

void nif_read_block_types(nif_t *nif)
{
	int n = hedr.num_block_types;
	hedr.block_types = malloc(n * sizeof(char *));
	for (int i = 0; i < n; i++)
	{
	hedr.block_types[i] = nif_read_sized_string(nif);
	}
}

void nif_read_block_type_index(nif_t *nif)
{
	int size = hedr.num_blocks * sizeof(unsigned short);
	hedr.block_type_index = malloc(size);
	memcpy(hedr.block_type_index, buf + pos, size);
	pos += size;
}

void nif_read_block_sizes(nif_t *nif)
{
	int size = hedr.num_blocks * sizeof(unsigned int);
	hedr.block_sizes = malloc(size);
	memcpy(hedr.block_sizes, buf + pos, size);
	pos += size;
}

void nif_read_strings(nif_t *nif)
{
	hedr.num_strings = from_buf();
	four();
	hedr.max_string_length = from_buf();
	four();
	int n = hedr.num_strings;
	hedr.strings = malloc(n * sizeof(char *));
	for (int i = 0; i < n; i++)
	{
	hedr.strings[i] = nif_read_sized_string(nif);
	}
}

void nif_read_groups(nif_t *nif)
{
	hedr.num_groups = from_buf();
	four();
}