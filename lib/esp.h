#ifndef LIB_ESP_H
#define LIB_ESP_H

// based on Ortham/libespm

#define api

#define SNPRINTF_FORM_ID 0

// weird things
// only capitalized type is esp struct
// offsets have the type unsigned (an unsigned int)

struct form_id;

struct esp;
struct grup;
struct record;
struct subrecord;

typedef struct esp esp;
typedef struct grup grup;
typedef struct record record;
typedef struct subrecord subrecord;

typedef esp Esp;

typedef struct revised_array
{
	void **elements;
	unsigned int size, capacity;
} revised_array;

struct esp
{
	char filename[260];
	void *stream;
	record *header;
	void *file;
	unsigned pos;
	unsigned filesize;
	int active;
	revised_array * grups, * records, * large;
	struct form_id *formIds;
	struct
	{
	unsigned int grups, records, subrecords, uncompress;
	} ids;
};


#pragma pack(push, 1)

struct grup_header
{
	unsigned int sgn, size, label;
	int group_type;
	unsigned short time_stamp, version_control_info;
	unsigned int unknown;
};

struct grup_header_small
{
	unsigned int sgn, size, label;
	int group_type;
};

struct record_header
{
	unsigned int sgn, size, flags, formId;
	unsigned short time_stamp, version_control_info, form_version, unknown;
};

struct record_header_small
{
	unsigned int sgn, size, flags, formId;
};

struct subrecord_header
{
	unsigned int sgn;
	unsigned short size;
};

typedef struct esp_dud {
	char x;
} esp_dud;

#pragma pack(pop)

struct form_id
{
	struct esp *esp;
	unsigned int formId, modIndex, objectIndex;
#if SNPRINTF_FORM_ID
	char hex[9];
#endif
};

struct grup
{
	char g;
	unsigned int id;
	unsigned offset;
	const struct grup_header *hed;
	unsigned char *data;
	struct esp *esp;
	revised_array * mixed;
	char unread;
};

struct record
{
	char r;
	unsigned int id;
	unsigned short indices;
	unsigned offset;
	const struct record_header *hed;
	revised_array *subrecords;
	struct form_id *form_id;
	struct esp *esp;
	char lazy;
	unsigned int til;
	// compression related
	char *buf;
	unsigned int size;
	unsigned pos;
};

struct subrecord
{
	char s;
	unsigned int id;
	unsigned offset;
	const struct subrecord_header *hed;
	//int skip;
	// buffer related
	char *buf;
	unsigned char *data;
};

typedef esp * espp;
typedef esp ** esppp;

typedef grup * grupp;
typedef grup ** gruppp;
typedef record * recordp;
typedef subrecord * subrecordp;

typedef recordp rcdp;
typedef subrecordp rcdbp;

typedef const esp * cespp;
typedef const grup * cgrupp;
typedef const record * crecordp;
typedef const subrecord * csubrecordp;

api espp plugin_load(const char *);

api void esp_read_lazy_grup(grupp);
api void esp_read_lazy_record(rcdp);

api void esp_print_form_id(espp, char *, struct form_id *);
api void esp_print_grup(espp, char *, grupp);
api void esp_print_record(espp, char *, recordp);
api void esp_print_field(espp, char *, subrecordp);

api esppp get_plugins();

api espp has_plugin(const char *);

api revised_array *esp_filter_objects(cespp, const char [5]);

api recordp esp_get_form_id(unsigned int);

api grupp esp_top_grup(cespp, const char [5]);

api void free_plugin(esppp);
api void free_esp_array(revised_array **);

#endif