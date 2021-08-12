#ifndef LIB_ESP_H
#define LIB_ESP_H

// based on Ortham/libespm

#define api

#define SNPRINTF_FORM_ID 0

#define PLUGINS_SAVE_OFFSETS 1

#define SUPPORT_XXX_LARGE_SUBRECORDS 1 // doesnt do anything yet

// positions have type unsigned

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
extern int esp_only_read_first_subrecord;

typedef struct revised_array
{
	void **elements;
	unsigned int size, capacity;
} revised_array;

struct esp
{
	const char *name;
	void *file;
	unsigned pos;
	const char *buf;
	unsigned filesize;
	int active;
	record *header;
	revised_array * grups, * records;
	struct form_id *formIds;
	struct
	{
	unsigned short grups, records, subrecords, uncompress;
	} count;
};

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
	const struct grup_header *hed;
	unsigned char *data;
	revised_array * mixed;
};

struct record
{
	char r;
	const struct record_header *hed;
#if PLUGINS_SAVE_OFFSETS
	unsigned offset;
#endif
	unsigned short indices;
	unsigned short id;
	revised_array *subrecords;
	unsigned char *data;
	unsigned int actualSize;
	struct form_id *form_id;
	esp *esp;
	char lazy;
	// compression related
	unsigned pos;
	char *buf;
};

struct subrecord
{
	char s;
	const struct subrecord_header *hed;
#if PLUGINS_SAVE_OFFSETS
	unsigned offset;
#endif
	unsigned int index;
	unsigned short id;
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

api espp plugin_slate();
api int plugin_load(espp);

api void esp_read_lazy_record(crecordp);

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