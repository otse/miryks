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

#define ESP Esp *
#define GRUP grup *
#define RCD record *
#define SRCD subrecord *

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
	char *buf;
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
	ESP esp;
	unsigned int formId, modIndex, objectIndex;
};

struct grup
{
	char g;
	unsigned int id;
	unsigned offset;
	const struct grup_header *hed;
	unsigned char *data;
	ESP esp;
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
	ESP esp;
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

api ESP plugin_load(const char *, int);

api void esp_check_grup(GRUP);
api void esp_check_rcd(RCD);

api void esp_print_form_id(ESP, char *, struct form_id *);
api void esp_print_grup(ESP, char *, GRUP);
api void esp_print_record(ESP, char *, RCD);
api void esp_print_field(ESP, char *, SRCD);

api ESP *get_plugins();

api ESP has_plugin(const char *);

api revised_array *esp_filter_objects(const ESP, const char [5]);

api RCD esp_get_form_id(unsigned int);

api GRUP esp_top(const ESP, const char [5]);

api void free_plugin(ESP *);
api void free_esp_array(revised_array **);

#endif