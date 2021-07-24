#ifndef ESP_H
#define ESP_H

// part of gloom

// only for le
// can easily be altered for se

// 100% based on https://github.com/Ortham/libespm

#define api

struct form_id;

struct grup_t;
struct record_t;
struct subrecord_t;

//typedef png_struct * png_structp;

#define Fields object->fields

#define espwrd *(unsigned int *)

extern int esp_skip_fields;

extern const char *esp_types[];

typedef struct EspCArray
{
	union{
	void **elements;
	struct grup_t **grups;
	struct record_t **records;
	struct subrecord_t **subrecords;
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
	struct record_t *header;
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
	struct record_t *record;
	void *plugin;
};

struct grup_header
{
	unsigned int type, size, label;
	int group_type;
	unsigned short time_stamp, version_control_info;
	unsigned int unknown;
};

struct record_header
{
	unsigned int type, size, flags, formId;
	unsigned short time_stamp, version_control_info, form_version, unknown;
};

struct field_header
{
	unsigned int type;
	unsigned short size;
};

typedef struct Dud {
	char x;
} Dud;

typedef struct grup_t
{
	char x;
	unsigned int id;
	const struct grup_header *hed;
	unsigned char *data;
	EspCArray mixed;
} grup_t;

typedef struct record_t
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
} record_t;

typedef struct subrecord_t
{
	char x;
	unsigned int index;
	unsigned int id;
	long offset;
	const struct field_header *hed;
	unsigned int actualSize;
	unsigned char *data;
} subrecord_t;

typedef grup_t * grupp;
typedef record_t * recordp;
typedef subrecord_t * subrecordp;

#pragma pack(pop)

api Esp *plugin_slate();
api int plugin_load(Esp *);

api void esp_print_form_id(Esp *, char *, struct form_id *);
api void esp_print_grup(Esp *, char *, grup_t *);
api void esp_print_record(Esp *, char *, record_t *);
api void esp_print_field(Esp *, char *, subrecord_t *);

api Esp **get_plugins();

api Esp *has_plugin(const char *);

api EspCArray *esp_filter_objects(const Esp *, const char [5]);

api record_t *esp_get_form_id(unsigned int);

api grup_t *esp_top_grup(const Esp *, const char [5]);

api void free_plugin(Esp **);
api void free_esp_array(EspCArray *);

#endif