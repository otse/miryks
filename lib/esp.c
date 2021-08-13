// see header for stuff

// warning program very ugly

#include "common.h"

#include "esp.h"

#include <zlib.h>
#include <sys/stat.h>

#define Buf esp->buf
#define Pos esp->pos

#define Count esp->count

int esp_skip_subrecords = 0; // fast option

int esp_only_read_first_subrecord = 1;

#define PLUGINS 8

static espp plugins[PLUGINS] = { NULL };

#define COUNT_OF(x) sizeof(x) / sizeof(0[x])

inline void narray(revised_array **, unsigned int);
inline void grow(revised_array *);
inline void insert(revised_array *, void *);

void make_form_ids(espp );

recordp read_record(espp);
grupp read_grup(espp);

unsigned int hedr_num_records(espp esp)
{
	if (esp_skip_subrecords)
	return 200;
	return *(unsigned int *)((*(subrecord ***)esp->header->subrecords)[0]->data + 4); // bad dont do this
}

espp plugin_slate()
{
	espp plugin = calloc(1, sizeof(esp));
	return plugin;
}

api int plugin_load(espp esp)
{
	assertm(esp->buf,      "need buf");
	assertm(esp->filesize, "need filesize");
	assertm(esp->name,     "need name");
	esp->header = read_record(esp);
	narray(&esp->grups, 200);
	narray(&esp->records, hedr_num_records(esp));
	while(Pos < esp->filesize)
	{
	grupp grp = read_grup(esp);
	insert(esp->grups, grp);
	}
	make_form_ids(esp);
	return 1;
}

void uncompress_record(espp, recordp);
inline void read_record_subrecords(espp, recordp);

recordp read_record(espp esp)
{
	recordp rec;
	rec = calloc(1, sizeof(record));
	// head
	rec->r = 'r';
	rec->id = Count.records++;
#if PLUGINS_SAVE_OFFSETS
	rec->offset = Pos;
#endif
	rec->hed = Buf + Pos;
	Pos += sizeof(struct record_header);
	Pos += 0;
	rec->actualSize = rec->hed->size;
	rec->data = Buf + Pos;
	rec->lazy = esp_only_read_first_subrecord;
	rec->esp = esp;
	narray(&rec->subrecords, 1);
	insert(esp->records, rec);
	// printf("R %.4s %u > ", (char *)&rec->hed->sgn, rec->hed->dataSize);
	// fields
	if ( esp_skip_subrecords )
	Pos += rec->hed->size;
	else
	{
	if (rec->hed->flags & 0x00040000)
	{
	rec->buf = 0; rec->pos = 0;
	uncompress_record(esp, rec);
	read_record_subrecords(esp, rec);
	Pos += rec->hed->size;
	}
	else
	read_record_subrecords(esp, rec);
	}
	return rec;
}

inline subrecordp read_subrecord(espp, recordp, unsigned int);

inline void read_record_subrecords(espp esp, recordp rec)
{
	unsigned *pos = &Pos;
	if (rec->hed->flags & 0x00040000)
	pos = &rec->pos;
	unsigned start = *pos;
	unsigned int large = 0;
	while(*pos - start < rec->actualSize)
	{
	subrecordp sub;
	sub = read_subrecord(esp, rec, large);
	large = 0;
	if (sub->hed->sgn == *(unsigned int *)"XXXX")
	large = *(unsigned int *)sub->data;
	else
	insert(rec->subrecords, sub);
	if ( rec->lazy )
	{
	*pos = start + rec->actualSize;
	break;
	}
	}
}

api void esp_read_lazy_record(recordp rec)
{
	if (rec->lazy)
	{
	//printf("reading lazy record\n");
	rec->lazy = 0;
	//rec->subrecords->size = 0;
	rec->esp->pos = rec->offset;
	rec->esp->pos += sizeof(struct record_header);
	read_record_subrecords(rec->esp, rec);
	}
}

inline subrecordp read_subrecord(espp esp, recordp rec, unsigned int override)
{
	unsigned *pos = &Pos;
	char *buf = Buf;
	if (rec->hed->flags & 0x00040000)
	{
	pos = &rec->pos;
	buf = rec->buf;
	}
	subrecordp sub;
	sub = calloc(1, sizeof(subrecord));
	// hed
	sub->s = 's';
	sub->index = rec->indices++;
	sub->id = Count.subrecords++;
#if PLUGINS_SAVE_OFFSETS
	sub->offset = Pos;
#endif
	sub->hed = buf + *pos;
	*pos += sizeof(struct subrecord_header);
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
	grupp grp = calloc(1, sizeof(grup));
	grp->g = 'g';
	grp->id = Count.grups++;
	grp->hed = Buf + Pos;
	Pos += sizeof(struct grup_header);
	Pos += 0;
	grp->data = Buf + Pos;
	narray(&grp->mixed, 1);
	// printf("G %.4s %u > ", (char *)&grup->hed->sgn, grup->hed->size);
	// records
	read_grup_records(esp, grp);
	return grp;
}

const unsigned int peek_type(espp esp)
{
	return *(unsigned *) (Buf + Pos);
}

inline void read_grup_records(espp esp, grupp grp)
{
	unsigned size = grp->hed->size - sizeof(struct grup_header);
	unsigned start = Pos;
	while (Pos - start < size)
	{
	if (peek_type(esp) == *(unsigned int *)"GRUP")
	insert(grp->mixed, read_grup(esp));
	else
	insert(grp->mixed, read_record(esp));
	}
}

//void make_top_grup_revisit

api grupp esp_top_grup(const espp esp, const char type[5])
{
	for (unsigned int i = 0; i < esp->grups->size; i++)
	{
	grupp grp = esp->grups->elements[i];
	if (*(unsigned int *)type == grp->hed->label)
	return esp->grups->elements[i];
	}
	return NULL;
}

// 439341069
// 4294967295

inline void build_form_id(struct form_id *form_id, unsigned int formId)
{
	form_id->formId = formId;
#if SNPRINTF_FORM_ID
	snprintf(form_id->hex, 9, "%08X", formId);
#endif
	form_id->modIndex = formId >> 24;
	form_id->objectIndex = formId & ~(form_id->modIndex << 24);
}

void make_form_ids(espp esp)
{
	esp->formIds = calloc(esp->records->size, sizeof(struct form_id));
	for (unsigned int i = 0; i < esp->records->size; i++)
	{
	recordp rec = esp->records->elements[i];
	build_form_id(&esp->formIds[i], rec->hed->formId);
	rec->form_id = &esp->formIds[i];
	}
}

api recordp esp_get_form_id(unsigned int formId)
{
	struct form_id form_id;
	build_form_id(&form_id, formId);
	espp esp = get_plugins()[form_id.modIndex];
	if (esp == NULL)
	return NULL;
	for (unsigned int j = 0; j < esp->records->size; j++)
	{
	recordp rec = esp->records->elements[j];
	if (rec->form_id->objectIndex == form_id.objectIndex)
	return rec;
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

api revised_array * esp_filter_objects(const espp esp, const char type[5])
{
	revised_array * filtered;
	narray(&filtered, 100);
	for (unsigned int i = 0; i < esp->records->size; i++)
	{
	record *record = esp->records->elements[i];
	if (record->hed->sgn == *(unsigned int *)type)
	insert(filtered, record);
	}
	return filtered;
}

void uncompress_record(espp esp, recordp rec)
{
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
	for (int i = PLUGINS; i-- > 0;)
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
	for (int i = PLUGINS; i --> 0; )
	if (esp == plugins[i])
	plugins[i] = NULL;
	for (unsigned int i = 0; i < esp->records->size; i++)
	{
	recordp record = esp->records->elements[i];
	if (record->hed->flags & 0x00040000)
	free(record->buf);
	free_esp_array(&record->subrecords);
	}
	free_esp_array(&esp->grups);
	free_esp_array(&esp->records);
	free(esp->buf);
	free(esp);
}

// Horrible growable c array stolen from GitHub

api void free_esp_array(revised_array ** array)
{
	free((*array)->elements);
}

inline void narray(revised_array ** p, unsigned int initial) {
	*p = malloc(sizeof(revised_array));
	revised_array * a = (*p);
	a->capacity = initial;
	a->size = 0;
	a->elements = calloc(a->capacity, sizeof(void *));
}

inline void grow(revised_array * a) {
	if (a->size != a->capacity)
	return;
	a->capacity *= 2;
	a->elements = realloc(a->elements, a->capacity * sizeof(void *));
}

inline void insert(revised_array * a, void * element) {
	if (a == NULL)
	return;
	grow(a);
	a->elements[a->size++] = element;
}