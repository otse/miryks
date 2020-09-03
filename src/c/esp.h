#ifndef C_ESP_H
#define C_ESP_H

#define api

typedef struct esp Esp;
typedef struct grup Grup;
typedef struct record Record;
typedef struct subrecord Subrecord;

struct record;
struct subrecord;
struct grup;

extern int esp_skip_subrecords;

struct esp_array
{
	void **array;
	size_t used;
	size_t size;
};

struct esp
{
	int x;
	void *file;
	long pos;
	unsigned char *buf;
	long filesize;
	char *path;
	struct record *header;
	struct esp_array grups, records, subrecords;
};

enum espnum
{
	GRUP, RECORD, SUBRECORD
};

#pragma pack(push, 1)

struct grup_head
{
	unsigned int type;
	unsigned int size;
};

struct record_head
{
	unsigned int type;
	unsigned int dataSize;
	unsigned int flags;
	unsigned int formId;
};

struct subrecord_head
{
	unsigned int type;
	unsigned short size;
};

struct grup
{
	enum espnum x;
	unsigned int id;
	struct grup_head *head;
	struct esp_array grups, records, mixed;
};

struct record
{
	enum espnum x;
	unsigned int id;
	struct record_head *head;
	struct esp_array subrecords;
};

struct subrecord
{
	enum espnum x;
	unsigned int id;
	struct subrecord_head *head;
	unsigned int actualSize;
	const char *data;
};

#pragma pack(pop)

void esp_gui();

api struct esp *esp_load(const char *);

api void esp_read_subrecord_data(struct esp *, struct subrecord *);

api void esp_print_grup(struct esp *, char *, struct grup *);
api void esp_print_record(struct esp *, char *, struct record *);
api void esp_print_subrecord(struct esp *, char *, struct subrecord *);

api void esp_free(struct esp **);

#endif