#ifndef LIB_ESP_H
#define LIB_ESP_H

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

// access element grup->mixed->elements[5] could be grup or record
// the union is for saving casts when you're sure
// it just doesnt make sense tho,
// can we hack the array operator as a generic offset
// that still look better than array.elements?

typedef struct esp_t
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
	struct
	{
	unsigned int grups, records, fields, uncompress;
	} count;
} esp_t;

#define GRUP 1
#define RECORD 2
#define SUBRECORD 3

#pragma pack(push, 1)

struct form_id
{
	struct esp_t *esp;
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

typedef esp_t * espp;
typedef esp_t ** esppp;

typedef grup_t * grupp;
typedef record_t * recordp;
typedef subrecord_t * subrecordp;

typedef const esp_t * cespp;

typedef const grup_t * cgrupp;
typedef const record_t * crecordp;
typedef const subrecord_t * csubrecordp;


#pragma pack(pop)

api espp plugin_slate();
api int plugin_load(espp );

api void esp_print_form_id(espp, char *, struct form_id *);
api void esp_print_grup(espp, char *, grupp);
api void esp_print_record(espp, char *, recordp);
api void esp_print_field(espp, char *, subrecordp);

api esppp get_plugins();

api espp has_plugin(const char *);

api EspCArray *esp_filter_objects(const espp, const char [5]);

api recordp esp_get_form_id(unsigned int);

api grupp esp_top_grup(const espp, const char [5]);

api void free_plugin(esppp);
api void free_esp_array(EspCArray *);

#endif