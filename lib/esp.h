#ifndef ESP_H
#define ESP_H

#define api

// wrote for dark2

struct form_id;
struct record;
struct subrecord;
struct grup;

#define Fields object->fields

#define espwrd *(unsigned int *)

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

typedef struct esp Esp;
typedef struct grup Grup;
typedef struct record Record;
typedef struct subrecord Subrecord;

struct esp
{
	const char *name;
	void *file;
	long pos;
	const char *buf;
	long filesize;
	int active;
	Record *header;
	struct esp_array grups, records;
	struct form_id *formIds;
	const char **tops;
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
	Esp *esp;
	unsigned int formId, modIndex, objectIndex;
	char hex[9];
	Record *record;
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
	unsigned int indices;
	unsigned int id;
	long offset;
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
	unsigned int index;
	unsigned int id;
	long offset;
	const struct field_header *hed;
	unsigned int actualSize;
	unsigned char *data;
};


#pragma pack(pop)

void esp_gui();

api Esp *plugin_slate();
api int plugin_load(Esp *);

api void esp_print_form_id(Esp *, char *, struct form_id *);
api void esp_print_grup(Esp *, char *, struct grup *);
api void esp_print_record(Esp *, char *, Record *);
api void esp_print_field(Esp *, char *, Subrecord *);

api Esp **get_plugins();

api Esp *has_plugin(const char *);

api struct esp_array *esp_filter_objects(const Esp *, const char [5]);

api Record *esp_brute_record_by_form_id(unsigned int);

api struct grup *esp_get_top_grup(const Esp *, const char [5]);

api void free_plugin(Esp **);
api void free_esp_array(struct esp_array *);

#endif