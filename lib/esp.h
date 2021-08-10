#ifndef LIB_ESP_H
#define LIB_ESP_H

// based on Ortham/libespm

#define api

struct form_id;

struct esp;
struct grup;
struct record;
struct subrecord;

typedef struct esp esp;
typedef struct grup grup;
typedef struct record record;
typedef struct subrecord subrecord;

extern int esp_skip_subrecords;

typedef struct revised_array
{
	void **elements;
	unsigned int size, capacity;
} revised_array;

struct esp
{
	const char *name;
	void *file;
	long pos;
	const char *buf;
	long filesize;
	int active;
	record *header;
	revised_array * grups, * records;
	struct form_id *formIds;
	struct
	{
	unsigned int grups, records, fields, uncompress;
	} count;
};

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
	unsigned int sgn, size, label;
	int group_type;
	unsigned short time_stamp, version_control_info;
	unsigned int unknown;
};

struct record_header
{
	unsigned int sgn, size, flags, formId;
	unsigned short time_stamp, version_control_info, form_version, unknown;
};

struct subrecord_header
{
	unsigned int sgn;
	unsigned short size;
};

typedef struct esp_dud {
	char x;
} esp_dud;

struct grup
{
	char g;
	unsigned int id;
	const struct grup_header *hed;
	unsigned char *data;
	revised_array * mixed;
};

struct record
{
	char r;
	unsigned int indices;
	unsigned int id;
	long offset;
	const struct record_header *hed;
	struct form_id *fi;
	revised_array *subrecords;
	unsigned char *data;
	unsigned int actualSize;
	// compression related
	long pos;
	char *buf;
};

struct subrecord
{
	char s;
	unsigned int index;
	unsigned int id;
	long offset;
	const struct subrecord_header *hed;
	unsigned int actualSize;
	unsigned char *data;
};

typedef esp * espp;
typedef esp ** esppp;

typedef grup * grupp;
typedef grup ** gruppp;

typedef record * recordp;

typedef subrecord * subrecordp;

typedef const esp * cespp;

typedef const grup * cgrupp;
typedef const record * crecordp;
typedef const subrecord * csubrecordp;


#pragma pack(pop)

api espp plugin_slate();
api int plugin_load(espp );

api void esp_print_form_id(espp, char *, struct form_id *);
api void esp_print_grup(espp, char *, grupp);
api void esp_print_record(espp, char *, recordp);
api void esp_print_field(espp, char *, subrecordp);

api esppp get_plugins();

api espp has_plugin(const char *);

api revised_array *esp_filter_objects(const espp, const char [5]);

api recordp esp_get_form_id(unsigned int);

api grupp esp_top_grup(const espp, const char [5]);

api void free_plugin(esppp);
api void free_esp_array(revised_array ** );

#endif