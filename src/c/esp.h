#ifndef C_ESP_H
#define C_ESP_H

#define api

typedef struct esp Esp;
typedef struct grup Grup;
typedef struct record Record;
typedef struct subrecord Subrecord;

struct form_id;
struct record;
struct subrecord;
struct grup;

#define GRUP_HEX 0x50555247
#define XXXX_HEX 0x58585858
#define STAT_HEX 0x54415453
#define EDID_HEX 0x44494445
#define FULL_HEX 0x4C4C5546

extern int esp_skip_subrecords;

struct esp_array
{
	union {
	void *array;
	void **pointers;
	};
	size_t used;
	size_t size;
	size_t element;
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
	struct esp_array formIds;
	struct esp_array grups, records;
};

enum espnum
{
	GRUP, RECORD, SUBRECORD
};

#pragma pack(push, 1)

#define FORMID_HEX 1

struct form_id
{
	unsigned int formId;
	char hex[9];
};

struct grup_head
{
	unsigned int type;
	unsigned int size;
};

struct record_head
{
	unsigned int type;
	unsigned int size;
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
	unsigned char *data;
	struct esp_array grups, records, mixed;
};

struct record
{
	enum espnum x;
	unsigned int id;
	struct record_head *head;
	unsigned char *data;
	struct esp_array fields;
	// compression related
	unsigned int actualSize;
	int pos;
	unsigned char *buf;
};

struct subrecord
{
	enum espnum x;
	unsigned int id;
	struct subrecord_head *head;
	unsigned int actualSize;
	unsigned char *data;
};

#pragma pack(pop)

void esp_gui();

api struct esp *esp_load(const char *);

api void esp_read_subrecord_data(struct esp *, struct subrecord *);

api void esp_print_grup(struct esp *, char *, struct grup *);
api void esp_print_record(struct esp *, char *, struct record *);
api void esp_print_subrecord(struct esp *, char *, struct subrecord *);

api struct esp_array *esp_filter_records(struct esp *, char s[4]);

api void free_esp(struct esp **);
api void free_esp_array(struct esp_array **);

#endif