#ifndef C_ESP_H
#define C_ESP_H

#define api

typedef struct esp Esp;
typedef struct record Record;

struct record;
struct subrecord;
struct group;

struct esp
{
	int x;
	void *stream;
	unsigned int pos;
	const char *path;
	struct record *header;
};

struct group
{
	char type[5];
	unsigned int size;
};

struct record
{
	char type[5];
	unsigned int flags, formId, subrecordsSize;
	char *sink;
	int pos;
	int count;
	struct subrecord **subrecords;
};

struct subrecord
{
	char type[5];
	unsigned int length;
	int start;
	unsigned char *buf;
	struct subrecord *next;
};



void esp_gui();

api struct esp *esp_load(const char *);

api void esp_read_subrecord_data(struct esp *, struct subrecord *);

api void esp_print_record(struct esp *, char *, struct record *);
api void esp_print_subrecord(struct esp *, char *, struct subrecord *);

api void esp_free(struct esp **);

#endif