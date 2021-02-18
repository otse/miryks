#ifndef ESP_H
#define ESP_H

// only for oldrim!

// wrote for gloom/dark
// 100% based on https://github.com/Ortham/libespm

// sorry but uses the words fields / subrecords interchangeably

#define api

struct form_id;

struct Grup;
struct Record;
struct Subrecord;

#define Fields object->fields

#define espwrd *(unsigned int *)

extern int esp_skip_fields;

extern const char *esp_types[];

typedef struct EspCArray
{
	union{
	void **elements;
	struct Grup **grups;
	struct Record **records;
	struct Subrecord **subrecords;
	};
	unsigned int size;
	unsigned int capacity;
} EspCArray;

typedef struct Esp
{
	const char *name;
	void *file;
	long pos;
	const char *buf;
	long filesize;
	int active;
	struct Record *header;
	EspCArray grups, records;
	struct form_id *formIds;
	const char **tops;
	struct
	{
	unsigned int grups, records, fields, uncompress;
	} count;
} Esp;

#define GRUP 1
#define RECORD 2
#define SUBRECORD 3

#pragma pack(push, 1)

struct form_id
{
	Esp *esp;
	unsigned int formId, modIndex, objectIndex;
	char hex[9];
	struct Record *record;
	void *plugin;
};

struct grup_header
{
	unsigned int type, size;
};

struct record_header
{
	unsigned int type, size, flags, formId;
};

struct field_header
{
	unsigned int type;
	unsigned short size;
};

typedef struct TypeDud {
	char x;
} TypeDud;

typedef struct Grup
{
	char x;
	unsigned int id;
	const struct grup_header *hed;
	unsigned char *data;
	EspCArray mixed;
} Grup;

typedef struct Record
{
	char x;
	unsigned int indices;
	unsigned int id;
	long offset;
	const struct record_header *hed;
	struct form_id *fi;
	EspCArray fields;
	unsigned char *data;
	unsigned int actualSize;
	// compression related
	long pos;
	char *buf;
} Record;

typedef struct Subrecord
{
	char x;
	unsigned int index;
	unsigned int id;
	long offset;
	const struct field_header *hed;
	unsigned int actualSize;
	unsigned char *data;
} Subrecord;

#pragma pack(pop)

api Esp *plugin_slate();
api int plugin_load(Esp *);

api void esp_print_form_id(Esp *, char *, struct form_id *);
api void esp_print_grup(Esp *, char *, Grup *);
api void esp_print_record(Esp *, char *, Record *);
api void esp_print_field(Esp *, char *, Subrecord *);

api Esp **get_plugins();

api Esp *has_plugin(const char *);

api EspCArray *esp_filter_objects(const Esp *, const char [5]);

api Record *esp_get_form_id(unsigned int);

api Grup *esp_get_top_grup(const Esp *, const char [5]);

api void free_plugin(Esp **);
api void free_esp_array(EspCArray *);

#endif