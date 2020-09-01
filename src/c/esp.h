#ifndef C_ESP_H
#define C_ESP_H

#define api

typedef struct esp Esp;
typedef struct record Record;

struct record;
struct subrecord;
struct grup;

enum espnum { GRUP, RECORD, SUBRECORD };

struct esp
{
	int x;
	void *stream;
	unsigned int pos, filesize;
	const char *path;
	struct record *header;
	int count;
	struct grup **grups;
};

struct grup
{
	enum espnum x;
	int id;
	int parent;
	char type[5];
	//unsigned int skip;
	unsigned int size;
};

struct record
{
	enum espnum x;
	int id;
	void *parent;
	char type[5];
	unsigned int dataSize;
	unsigned int flags;
	unsigned int formId;
	char *sink;
	int pos;
	int count;
	struct subrecord **subrecords;
};

struct subrecord
{
	enum espnum x;
	int id;
	void *parent;
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