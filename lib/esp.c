// see header for stuff

// warning program very ugly

#include "common.h"

#include "esp.h"

#include <zlib.h>
#include <sys/stat.h>

int esp_only_read_first_subrecord = 1;

#define PLUGINS 8

static Esp *plugins[PLUGINS] = { NULL };

// horrible c vector
inline void narray(revised_array **, unsigned int);
inline void grow(revised_array *);
inline void insert(revised_array *, void *);

#define Buf esp->buf
#define Pos esp->pos
#define Ids esp->ids

void make_form_ids(Esp *);

recordp read_record(Esp *);
grupp read_grup(Esp *);

unsigned int hedr_num_records(Esp *esp)
{
	return *(unsigned int *)((*(subrecord ***)esp->header->subrecords)[0]->data + 4); // bad dont do this
}

api Esp *plugin_load(const char *path)
{
	Esp *esp = calloc(1, sizeof(esp));
	file_name(esp->filename, path, '/');
	esp->stream = fopen(path, "rb");
	assertm(
		esp->stream, path);
	printf("goign to read the esp header\n");
	esp->header = read_record(esp);
	narray(&esp->grups, 200);
	narray(&esp->records, hedr_num_records(esp));
	narray(&esp->large, 10);
	while(Pos < esp->filesize)
	{
	grupp grp = read_grup(esp);
	insert(esp->grups, grp);
	}
	make_form_ids(esp);
	return 1;
}

void uncompress_record(Esp *, recordp);
inline void process_record(Esp *, recordp);

recordp read_record(Esp *esp)
{
	printf("read rcd\n");
	recordp rcd = calloc(1, sizeof(record));

	rcd->r = 'r';
	rcd->id = Ids.records++;
	rcd->lazy = esp_only_read_first_subrecord;
	rcd->esp = esp;

	read(esp, &rcd->hed, sizeof(struct record_header));

	rcd->offset = tell(esp);

	printf("read rcd sgn %.4s\n", (char *)&rcd->hed.sgn);

	rcd->size = rcd->hed.size;

	//narray(&rcd->subrecords, 1);
	//insert(esp->records, rcd);
	
	rcd->buf = malloc(rcd->hed.size);
	read(esp, rcd->buf, rcd->hed.size);
	
	if (rcd->hed.flags & 0x00040000)
	{
	uncompress_record(esp, rcd);
	process_record(esp, rcd);
	}
	else
	process_record(esp, rcd);
	
	return rcd;
}

inline subrecordp read_sub(Esp *, recordp);

inline void process_record(Esp *esp, recordp rcd)
{
	printf("process record\n");
	unsigned begin = rcd->pos;

	unsigned int large = 0;

	while(rcd->pos - begin < rcd->size)
	{
		subrecordp sub = read_sub(esp, rcd);

		if (sub->hed.sgn == *(unsigned int *)"XXXX")
		{
		large = *(unsigned int *)sub->data;
		rcd->pos += large;
		}

		insert(rcd->subrecords, sub);
		
		if ( rcd->lazy )
			break;
	}
	seek(esp, rcd->offset + rcd->size);
}

api void esp_read_lazy_record(recordp rcd)
{
	if (rcd->lazy)
	{
	rcd->lazy = 0;
	rcd->esp->pos = rcd->offset;
	rcd->esp->pos += sizeof(struct record_header);
	process_record(rcd->esp, rcd);
	}
}

inline subrecordp read_sub(Esp *esp, recordp rcd)
{
	printf("read sub\n");

	subrecordp sub;
	sub = calloc(1, sizeof(subrecord));
	sub->s = 's';
	sub->id = Ids.subrecords++;
	sub->offset = Pos;
	//sub->hed = buf + *pos;
	rcd->pos += sizeof(struct subrecord_header);
	sub->data = buf + *pos;
	*pos += sub->hed.size;
	return sub;
}

inline void read_grup_records(Esp *, grupp);


grupp read_grup(Esp *esp)
{
	grupp grp = calloc(1, sizeof(grup));
	grp->g = 'g';
	grp->id = Ids.grups++;
	//grp->hed = Buf + Pos;
	Pos += sizeof(struct grup_header);
	Pos += 0;
	grp->data = Buf + Pos;
	narray(&grp->mixed, 1);

	read_grup_records(esp, grp);
	return grp;
}

const unsigned int peek_type(Esp *esp)
{
	return *(unsigned *) (Buf + Pos);
}

inline void read_grup_records(Esp *esp, grupp grp)
{
	unsigned size = grp->hed.size - sizeof(struct grup_header);
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

api grupp esp_top_grup(const Esp *esp, const char type[5])
{
	for (unsigned int i = 0; i < esp->grups->size; i++)
	{
	grupp grp = esp->grups->elements[i];
	if (*(unsigned int *)type == grp->hed.label)
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

void make_form_ids(Esp *esp)
{
	esp->formIds = calloc(esp->records->size, sizeof(struct form_id));
	for (unsigned int i = 0; i < esp->records->size; i++)
	{
	recordp rcd = esp->records->elements[i];
	build_form_id(&esp->formIds[i], rcd->hed.formId);
	rcd->form_id = &esp->formIds[i];
	}
}

api recordp esp_get_form_id(unsigned int formId)
{
	struct form_id form_id;
	build_form_id(&form_id, formId);
	Esp *esp = get_plugins()[form_id.modIndex];
	if (esp == NULL)
	return NULL;
	for (unsigned int j = 0; j < esp->records->size; j++)
	{
	recordp rcd = esp->records->elements[j];
	if (rcd->form_id->objectIndex == form_id.objectIndex)
	return rcd;
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

api revised_array * esp_filter_objects(const Esp *esp, const char type[5])
{
	revised_array * filtered;
	narray(&filtered, 100);
	for (unsigned int i = 0; i < esp->records->size; i++)
	{
	record *record = esp->records->elements[i];
	if (record->hed.sgn == *(unsigned int *)type)
	insert(filtered, record);
	}
	return filtered;
}

void uncompress_record(Esp *esp, recordp rcd)
{
	printf("uncompress");
	// char *src = rcd->data;
	// dont use rcd->data, use rcd->offset + sizeof header ?
	char *src = rcd->buf;
	const unsigned int realSize = *(unsigned int *)src;
	const unsigned int size = rcd->hed.size - 4;
	src += 4;
	rcd->data = malloc(realSize * sizeof(char));
	int ret = uncompress(rcd->buf, (uLongf*)&realSize, src, size);
	assertm(ret == Z_OK, "esp zlib");
	rcd->size = realSize;
	Ids.uncompress++;
}

api Esp **get_plugins()
{
	return plugins;
}

api Esp *has_plugin(const char *name)
{
	for (int i = PLUGINS; i-- > 0;)
	if (plugins[i] != NULL && 0 == strcmp(name, plugins[i]->filename))
	return plugins[i];
	return NULL;
}

api void free_plugin(Esp **p)
{
	Esp *esp = *p;
	*p = NULL;
	if (esp == NULL)
	return;
	for (int i = PLUGINS; i --> 0; )
	if (esp == plugins[i])
	plugins[i] = NULL;
	for (unsigned int i = 0; i < esp->records->size; i++)
	{
	recordp rcd = esp->records->elements[i];
	if (rcd->hed.flags & 0x00040000)
	free(rcd->buf);
	free_esp_array(&rcd->subrecords);
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

static int read(Esp *esp, void *data, size_t count)
{
	return fread(data, 1, count, (FILE *)esp->stream);
}

static int seek(Esp *esp, long int offset)
{
	return fseek((FILE *)esp->stream, offset, SEEK_SET);
}

static long int tell(Esp *esp)
{
	return ftell((FILE *)esp->stream);
}