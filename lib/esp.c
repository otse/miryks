// see header for stuff

#include "common.h"

#include "esp.h"

#include <zlib.h>
#include <sys/stat.h>

// subrecord *read_subrecord ?
recordp read_record(espp );
grupp read_grup(espp );

#define Buf esp->buf
#define Pos esp->pos

#define Count esp->count

int esp_skip_fields = 0;

static espp plugins[5] = { NULL, NULL, NULL, NULL, NULL };

#define COUNT_OF(x) sizeof(x) / sizeof(0[x])

inline void array(EspCArray *, unsigned int);
inline void grow(EspCArray *);
inline void insert(EspCArray *, void *);

void make_form_ids(espp );

unsigned int hedr_num_records(espp esp)
{
	if (esp_skip_fields)
	return 200;
	return *(unsigned int *)(esp->header->fields.subrecords[0]->data + 4);
}

espp plugin_slate()
{
	espp plugin = malloc(sizeof(esp));
	memset(plugin, 0, sizeof(esp));
	return plugin;
}

api int plugin_load(espp esp)
{
	assertm(esp->buf,      "need buf");
	assertm(esp->filesize, "need filesize");
	assertm(esp->name,     "need name");
	esp->header = read_record(esp);
	array(&esp->grups, 200);
	array(&esp->records, hedr_num_records(esp));
	while(Pos < esp->filesize)
	{
	grupp grp = read_grup(esp);
	insert(&esp->grups, grp);
	}
	make_form_ids(esp);
	return 1;
}

void uncompress_record(espp, recordp);
inline void read_record_fields(espp, recordp);

recordp read_record(espp esp)
{
	// Todo, clean big unclear assignments lik these thruout the program
	recordp rec;
	rec = malloc(sizeof(record));
	rec->fi = NULL;
	// head
	rec->x = RECORD;
	rec->indices = 0;
	rec->id = Count.records++;
	rec->offset = Pos;
	rec->hed = Buf + Pos;
	Pos += sizeof(struct record_header);
	Pos += 0;
	rec->actualSize = rec->hed->size;
	rec->data = Buf + Pos;
	array(&rec->fields, 6);
	insert(&esp->records, rec);
	// printf("R %.4s %u > ", (char *)&rec->hed->sgn, rec->hed->dataSize);
	// fields
	if (esp_skip_fields)
	Pos += rec->hed->size;
	else
	{
	if (rec->hed->flags & 0x00040000)
	{
	rec->buf = 0; rec->pos = 0;
	uncompress_record(esp, rec);
	read_record_fields(esp, rec);
	Pos += rec->hed->size;
	}
	else
	read_record_fields(esp, rec);
	}
	return rec;
}

inline subrecordp read_field(espp, recordp, unsigned int);

inline void read_record_fields(espp esp, recordp rec)
{
	long *pos = &Pos;
	if (rec->hed->flags & 0x00040000)
	pos = &rec->pos;
	long start = *pos;
	unsigned int large = 0;
	while(*pos - start < rec->actualSize)
	{
	subrecordp sub;
	sub = read_field(esp, rec, large);
	large = 0;
	if (sub->hed->sgn == *(unsigned int *)"XXXX")
	large = *(unsigned int *)sub->data;
	else
	insert(&rec->fields, sub);
	}
}

inline subrecordp read_field(espp esp, recordp rec, unsigned int override)
{
	long *pos = &Pos;
	char *buf = Buf;
	if (rec->hed->flags & 0x00040000)
	{
	pos = &rec->pos;
	buf = rec->buf;
	}
	subrecordp sub;
	sub = malloc(sizeof(subrecord));
	// hed
	sub->x = SUBRECORD;
	sub->index = rec->indices++;
	sub->id = Count.fields++;
	sub->offset = Pos;
	sub->hed = buf + *pos;
	*pos += sizeof(struct field_header);
	sub->actualSize = override == 0 ? sub->hed->size : override;
	// data
	sub->data = buf + *pos;
	*pos += sub->actualSize;
	// printf("S %.4s %u > ", (char *)&sub->hed->sgn, sub->hed->size);
	return sub;
}

inline void read_grup_records(espp, grupp);


grupp read_grup(espp esp)
{
	grupp grp = malloc(sizeof(grup));
	//grup->lowest = grup->highest = 0;
	// hed
	grp->x = GRUP;
	grp->id = Count.grups++;
	grp->hed = Buf + Pos;
	Pos += sizeof(struct grup_header);
	Pos += 0;
	grp->data = Buf + Pos;
	array(&grp->mixed, 12);
	// printf("G %.4s %u > ", (char *)&grup->hed->sgn, grup->hed->size);
	// records
	read_grup_records(esp, grp);
	// printf("\nend grup\n");
	return grp;
}

const unsigned int peek_type(espp esp)
{
	return *(unsigned int *) (Buf + Pos);
}

inline void read_grup_records(espp esp, grupp grp)
{
	long size = grp->hed->size - sizeof(struct grup_header) - 16;
	long start = Pos;
	while (Pos - start < size)
	{
	if (peek_type(esp) == *(unsigned int *) "GRUP")
	insert(&grp->mixed, read_grup(esp));
	else
	insert(&grp->mixed, read_record(esp));
	}
}

//void make_top_grup_revisit

api grupp esp_top_grup(const espp esp, const char type[5])
{
	for (unsigned int i = 0; i < esp->grups.size; i++)
	{
	grupp grp = esp->grups.elements[i];
	if (*(unsigned int *)type == grp->hed->label)
	return esp->grups.elements[i];
	}
	return NULL;
}

inline void build_form_id(espp esp, recordp record, struct form_id *fi)
{
	fi->esp = esp;
	record->fi = fi;
	fi->formId = record->hed->formId;
	fi->record = record;
	//snprintf(form_id.hex, 10, "%08X", record->hed->formId);
	//form_id.hex = 0;
	fi->modIndex = fi->formId >> 24;
	fi->objectIndex = fi->formId & ~(fi->modIndex << 24);
	//if (modIndex >= masters.size())
	//fi->plugin = parentPluginName;
	//else
	//fi->plugin = masters[fi->modIndex];
}

void make_form_ids(espp esp)
{
	// Unused
	EspCArray *array = &esp->records;
	esp->formIds = malloc(sizeof(struct form_id) * array->size);
	for (unsigned int i = 0; i < array->size; i++)
	build_form_id(esp, array->elements[i], &esp->formIds[i]);
}

api recordp esp_get_form_id(unsigned int formId)
{
	for (int i = 5; i --> 0; )
	{
	espp esp = plugins[i];
	if (esp == NULL)
	continue;
	for (unsigned int j = 0; j < esp->records.size; j++)
	{
	recordp rec = esp->records.elements[j];
	if (rec->fi->formId == formId)
	return rec;
	}
	}
	return NULL;
}

/*
api void esp_array_loop(EspCArray *array, void (*func)(subrecord *field, void *data), void *data)
{
	for (int i = 0; i < array->size; i++)
	{
	func(array->elements[i], data);
	};
}
*/

// Util code I did for imgui, kinda useless once I learned about top grups

api EspCArray *esp_filter_objects(const espp esp, const char type[5])
{
	EspCArray *filtered;
	filtered = malloc(sizeof(EspCArray));
	array(filtered, 100);
	for (unsigned int i = 0; i < esp->records.size; i++)
	{
	record *record = esp->records.elements[i];
	if (record->hed->sgn == *(unsigned int *)type)
	insert(filtered, record);
	}
	return filtered;
}

void uncompress_record(espp esp, recordp rec)
{
	// Workhorse code that is similar to how Bsa does it
	char *src = rec->data;
	const unsigned int realSize = *(unsigned int *)src;
	const unsigned int size = rec->hed->size - 4;
	src += 4;
	rec->buf = malloc(realSize * sizeof(char));
	int ret = uncompress(rec->buf, (uLongf*)&realSize, src, size);
	assertm(ret == Z_OK, "esp zlib");
	rec->actualSize = realSize;
	Count.uncompress++;
}

api esppp get_plugins()
{
	return plugins;
}

api espp has_plugin(const char *name)
{
	for (int i = 5; i-- > 0;)
	if (plugins[i] != NULL && 0 == strcmp(name, plugins[i]->name))
	return plugins[i];
	return NULL;
}

api void free_plugin(esppp p)
{
	espp esp = *p;
	*p = NULL;
	if (esp == NULL)
	return;
	for (int i = 5; i --> 0; )
	if (esp == plugins[i])
	plugins[i] = NULL;
	for (unsigned int i = 0; i < esp->records.size; i++)
	{
	recordp record = esp->records.elements[i];
	if (record->hed->flags & 0x00040000)
	free(record->buf);
	free_esp_array(&record->fields);
	}
	free_esp_array(&esp->grups);
	free_esp_array(&esp->records);
	free(esp->buf);
	free(esp);
}

// Horrible growable c array stolen from GitHub

api void free_esp_array(EspCArray *array)
{
	free(array->elements);
}

inline void array(EspCArray *a, unsigned int initial) {
	a->capacity = initial;
	a->size = 0;
	a->elements = malloc(a->capacity * sizeof(void *));
}

inline void grow(EspCArray *a) {
	if (a->size != a->capacity)
	return;
	a->capacity *= 2;
	a->elements = realloc(a->elements, a->capacity * sizeof(void *));
}

inline void insert(EspCArray *a, void *element) {
	grow(a);
	a->elements[a->size++] = element;
}