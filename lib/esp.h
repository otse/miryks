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

extern int esp_skip_subrecords;
extern int esp_only_read_first_subrecord;

typedef struct revised_array
{
	void **elements;
	unsigned int size, capacity;
} revised_array;

struct esp
{
	char filename[100];
	void *stream;
	record *header;
	void *file;
	unsigned pos;
	const char *buf;
	unsigned filesize;
	int active;
	revised_array * grups, * records, * large;
	struct form_id *formIds;
	struct
	{
	unsigned short grups, records, subrecords, uncompress;
	} ids;
};

typedef esp Esp;

#pragma pack(push, 1)

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
	unsigned short id;
	const struct grup_header hed;
	unsigned char *data;
	revised_array * mixed;
};

struct record
{
	char *read;
	char r;
	unsigned short id;
	unsigned short indices;
	unsigned offset;
	const struct record_header hed;
	revised_array *subrecords;
	struct form_id *form_id;
	Esp *esp;
	char lazy;
	// compression related
	char *buf;
	char *data;
	unsigned int size;
	unsigned pos;
};

struct subrecord
{
	char s;
	unsigned short id;
	unsigned offset;
	const struct subrecord_header hed;
	unsigned char *data;
};

typedef grup * grupp;
typedef grup ** gruppp;
typedef record * recordp;
typedef subrecord * subrecordp;

typedef const esp * cespp;
typedef const grup * cgrupp;
typedef const record * crecordp;
typedef const subrecord * csubrecordp;

api Esp *plugin_slate();
api Esp *plugin_load(const char *);

api void esp_read_lazy_record(crecordp);

api void esp_print_form_id(Esp *, char *, struct form_id *);
api void esp_print_grup(Esp *, char *, grupp);
api void esp_print_record(Esp *, char *, recordp);
api void esp_print_field(Esp *, char *, subrecordp);

api Esp **get_plugins();

api Esp *has_plugin(const char *);

api revised_array *esp_filter_objects(const Esp *, const char [5]);

api recordp esp_get_form_id(unsigned int);

api grupp esp_top_grup(const Esp *, const char [5]);

api void free_plugin(Esp **);
api void free_esp_array(revised_array ** );

#endif