#ifndef LIB_ESP_H
#define LIB_ESP_H

// based on Ortham/libespm

#define api

#define PLUGINS 8

// weird things
// only capitalized type is esp struct
// we use type `unsigned` for offsets

// also, a rcd only uses a buf when its compressed
// otherwise, we use the disk

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
	char *buf;
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
	unsigned int size;
	char looped;
};

struct record
{
	char r;
	unsigned int id;
	unsigned short indices;
	unsigned offset;
	const struct record_header *hed;
	revised_array *rcdbs;
	struct form_id form_id;
	struct esp *esp;
	char partial;
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

api espp plugin_load(const char *, int mem);

api void esp_check_grup(grupp);
api void esp_check_rcd(rcdp);

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