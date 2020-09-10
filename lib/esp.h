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

#define Fields object->fields

extern int esp_skip_fields;

extern const char *esp_types[];

struct esp_array
{
	union{
	void **elements;
	struct grup **grups;
	struct record **records;
	struct subrecord **subrecords;
	};
	size_t size;
	size_t capacity;
};

struct esp
{
	char path[260];
	void *file;
	long pos;
	char *buf;
	long filesize;
	struct record *header;
	struct esp_array grups, records;
	struct form_id *formIds;
	const char **tops;
	//const char **masters;
	struct
	{
	unsigned int grups, records, fields, uncompress;
	} count;
};

#define GRUP 1
#define RECORD 2
#define SUBRECORD 3

#pragma pack(push, 1)

struct form_id
{
	struct esp *esp;
	unsigned int formId, modIndex, objectIndex;
	char hex[9];
	struct record *record;
	void *plugin;
};

struct grup_header
{
	unsigned int type;
	unsigned int size;
};

struct record_header
{
	unsigned int type;
	unsigned int size;
	unsigned int flags;
	unsigned int formId;
};

struct field_header
{
	unsigned int type;
	unsigned short size;
};

struct grup
{
	char x;
	unsigned int id;
	const struct grup_header *hed;
	unsigned char *data;
	struct esp_array mixed;
};

struct record
{
	char x;
	unsigned int id;
	const struct record_header *hed;
	struct form_id *fi;
	struct esp_array fields;
	unsigned char *data;
	unsigned int actualSize;
	// compression related
	long pos;
	char *buf;
};

struct subrecord
{
	char x;
	unsigned int id;
	const struct field_header *hed;
	unsigned int actualSize;
	unsigned char *data;
};


#pragma pack(pop)

void esp_gui();

api struct esp *esp_load(const char *, int);

api void esp_print_form_id(struct esp *, char *, struct form_id *);
api void esp_print_grup(struct esp *, char *, struct grup *);
api void esp_print_record(struct esp *, char *, struct record *);
api void esp_print_field(struct esp *, char *, struct subrecord *);

api struct esp **get_plugins();

api struct esp_array *esp_lazy_filter(const struct esp *, const char [5]);

api struct record *esp_brute_record_by_form_id(unsigned int);
api struct grup *esp_get_top_grup(const struct esp *, const char [5]);

api void free_plugin(struct esp **);
api void free_esp_array(struct esp_array *);

#endif