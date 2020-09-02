#ifndef C_ESP_H
#define C_ESP_H

#define api

typedef struct esp Esp;
typedef struct record Record;

struct record;
struct subrecord;
struct grup;

extern int esp_skip_subrecords;


struct esp
{
	int x;
	void *file;
	long pos;
	unsigned char *buf;
	long filesize;
	char *path;
	struct record *header;
	int count;
	struct grup **grups;
};

#pragma pack(push, 1)

struct grup
{
	unsigned int type;
	unsigned int size;
	int end;
	int id;
};

struct record
{
	unsigned int type;
	unsigned int dataSize;
	unsigned int flags;
	unsigned int formId;
	int end;
	int id;
	char *sink;
	int pos;
	struct subrecord **subrecords;
};

struct subrecord
{
	unsigned int type;
	unsigned int size;
	int end;
	int id;
	long offset;
	unsigned char *buf;
};

#pragma pack(pop)

void esp_gui();

api struct esp *esp_load(const char *);
api struct esp *esp_load_in_memory(const char *);

api void esp_read_subrecord_data(struct esp *, struct subrecord *);

api void esp_print_record(struct esp *, char *, struct record *);
api void esp_print_subrecord(struct esp *, char *, struct subrecord *);

api void esp_free(struct esp **);

#endif