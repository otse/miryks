// see header for stuff

// based on the much more clearer Ortham/libespm

#include "common.h"

#include "esp.h"

#include <zlib.h>
#include <sys/stat.h>

int only_read_editorid = 1;
int skip_field_data = 1;

#if 0
#define spamf(...) printf(__VA_ARGS__);
#else
#define spamf 0;
#endif

#define PLUGINS 8

static espp plugins[PLUGINS] = { NULL };

// horrible c vector
inline void narray(revised_array **, unsigned int);
inline void grow(revised_array *);
inline void insert(revised_array *, void *);


static int disk_read(espp, void **, size_t);
static int disk_seek(espp, long int);
static long int disk_tell(espp);

#define Buf esp->buf
#define Pos esp->pos

void make_form_ids(espp);

rcdp read_record(espp);
grupp read_grup(espp, grupp);

unsigned int hedr_num_records(espp esp)
{
	rcdbp hedr = (*(subrecord ***)esp->header->subrecords)[0];
	float version = *(float *)(hedr->data);
	unsigned int num_records = *(unsigned int *)(hedr->data + 4);
	// printf("hedr tes4 version %f\n", version);
	return num_records;
}

api espp plugin_load(const char *path)
{
	espp esp = calloc(1, sizeof(struct esp));
	file_name(esp->filename, path, '/');
	esp->stream = fopen(path, "rb");
	assertm(
		esp->stream, path);
	fseek(esp->stream, 0L, SEEK_END);
	esp->filesize = ftell(esp->stream);
	rewind(esp->stream);
	spamf("goign to read the esp header\n");
	skip_field_data = 0;
	esp->header = read_record(esp);
	skip_field_data = 1;
	narray(&esp->grups, hedr_num_records(esp));
	narray(&esp->records, 400);
	narray(&esp->large, 10);
	int num = 0;
	while(Pos < esp->filesize)
	{
	grupp grp = read_grup(esp, 0);
	insert(esp->grups, grp);
	}
	spamf("last\n");
	make_form_ids(esp);
	return esp;
}


char *uncompress_record(espp_rcdp);
void loop_rcd(espp_rcdp);

rcdp read_record(espp esp)
{
	spamf("read rcd\n");

	rcdp rcd = calloc(1, sizeof(struct record));

	rcd->r = 'r';
	rcd->id = esp->ids.records++;
	rcd->lazy = only_read_editorid;
	rcd->esp = esp;
	disk_read(esp, &rcd->hed, sizeof(struct record_header));
	rcd->offset = Pos;
	rcd->size = rcd->hed->size;

	spamf("rcd - sgn, size %.4s %u\n", (char *)&rcd->hed->sgn, rcd->hed->size);
	if (rcd->hed->flags & 0x00040000)
	{
		//printf("rcd %.4s is compressed\n", (char *)&rcd->hed->sgn);
		/*
		disk_read(esp, &rcd->buf, rcd->size);
		uncompress_record(esp, rcd);
		printf("real size %u\n", rcd->size);
		loop_rcd(esp, rcd);
		//skip()
		*/
		//disk_seek(esp, rcd->offset + rcd->size);
	}
	else
		loop_rcd(esp, rcd);
	disk_seek(esp, rcd->offset + rcd->size);
	return rcd;
}

#define espp_rcdp espp esp, rcdp rcd

void skip(espp_rcdp, size_t);
void read(espp_rcdp, rcdbp, void **, size_t);

rcdbp read_rcdb(espp_rcdp);

static rcdbp rcdb_temp[40000];

void loop_rcd(espp_rcdp)
{
	spamf("process record\n");
	if (rcd->buf)
		rcd->pos = 0;
	else
		rcd->pos = rcd->offset;
	unsigned begin = rcd->pos;
	unsigned int num = 0;
	//static rcdbp temp[15000]; // should this be static? where will it reside? in stack? in address space?
	while(rcd->pos - begin < rcd->size)
	{
		assertm(num < 40000, "overflow");
		rcdbp rcdb = read_rcdb(esp, rcd);
		if (rcdb->hed->sgn == *(unsigned int *)"XXXX")
		{
			printf("\nuh no\n\n");
			rcdbp discard = read_rcdb(esp, rcd);
			skip(esp, rcd, *(unsigned int *)rcdb->data);
		}
		rcdb_temp[num++] = rcdb;
		if (rcd->lazy)
			break;
	}
	narray(&rcd->subrecords, num);
	for (unsigned int i = 0; i < num; i++)
		insert(rcd->subrecords, rcdb_temp[i]);
}

api void esp_read_lazy_grup(grupp grp)
{
	printf("read lazy grup\n");
	if (grp->unread == 1)
	{
		grp->unread = 0;
		printf("attempt read yet unread grup type %i !\n", grp->hed->group_type);
		//loop_grup(grp->esp, grp);
	}
}

api void esp_read_lazy_record(rcdp rcd)
{
	if (rcd->lazy)
	{
		rcd->lazy = 0;
		// dont forget to seek there first ?
		loop_rcd(rcd->esp, rcd);
	}
}

void read(espp_rcdp, void **dest, size_t size)
{
	if (rcd->buf)
		*dest = rcd->buf + rcd->pos;
	else
		disk_read(esp, dest, size);
	rcd->pos += size;
}

void skip(espp_rcdp, size_t n)
{
	// printf("skip %u\n", n);
	if (!rcd->buf)
	{
		Pos += n;
		disk_seek(esp, Pos);
	}
	rcd->pos += n;
}

rcdbp read_rcdb_data(espp_rcdp, rcdbp, int);

rcdbp read_rcdb(espp_rcdp)
{
	// printf("read rcdb\n");
	rcdbp rcdb = calloc(1, sizeof(struct subrecord));
	rcdb->s = 's';
	rcdb->id = esp->ids.subrecords++;
	read(esp, rcd, &rcdb->hed, sizeof(struct subrecord_header));
	if (rcd->buf)
	printf("rcdb - sgn, size %.4s %hu\n", (char *)&rcdb->hed->sgn, rcdb->hed->size);
	rcdb->offset = rcd->buf ? rcd->pos : Pos;
	read_rcdb_data(esp, rcd, rcdb, skip_field_data);
#if 0
	if (rcdb->hed->sgn == *(unsigned int *)"MAST")
		printf("MAST %s\n", (char *)rcdb->data);
#endif
	return rcdb;
}

rcdbp read_rcdb_data(espp_rcdp, rcdbp rcdb, int no)
{
	if (no)
	{
		//printf("skip rcdb data\n");
		skip(esp, rcd, rcdb->hed->size);
	}
	else
	{
		rcdb->data = malloc(rcdb->hed->size);
		read(esp, rcd, &rcdb->data, rcdb->hed->size);
	}
	return rcdb;
}

void loop_grup(espp, grupp);

grupp read_grup(espp esp, grupp parent)
{
	spamf("read_grup\n");
	grupp grp = calloc(1, sizeof(struct grup));
	grp->g = 'g';
	grp->id = esp->ids.grups++;
	disk_read(esp, &grp->hed, sizeof(struct grup_header));
	if (!parent)
		spamf("top grp %u %.4s\n", esp->grups->size, (char *)&grp->hed->label);
	grp->offset = Pos;
	loop_grup(esp, grp);
	return grp;
}

unsigned int peek_type(espp esp)
{
	unsigned int *sgn;
	disk_read(esp, &sgn, sizeof(unsigned int));
	disk_seek(esp, Pos - 4);
	return *sgn;
}

static void *grp_temp[40000]; // dialogs (DIAL) may reach 25,000 ish

void loop_grup(espp esp, grupp grp)
{
	unsigned size = grp->hed->size - sizeof(struct grup_header);
	unsigned start = Pos;
	if (grp->hed->group_type >= 2 && grp->hed->group_type <= 5)
	{
		//printf("skip grp\n");
		if (grp->unread == 0)
			grp->unread = 1;
		disk_seek(esp, grp->offset + size);
	}
	//static void *temp[30000]; // dialogs (DIAL) may reach 25,000 ish
	unsigned int num = 0;
	while (Pos - start < size)
	{
	assertm(num < 40000, "overflow");
	//if (num>10000)
	//	printf("reached 10000??");
	if (peek_type(esp) == *(unsigned int *)"GRUP")
	{
		grupp grp2 = read_grup(esp, grp);
		grp_temp[num++] = grp2;
	}
	else
	{
		rcdp rcd = read_record(esp);
		grp_temp[num++] = rcd;
	}
	}
	narray(&grp->mixed, num);
	for (unsigned int i = 0; i < num; i++)
		insert(grp->mixed, grp_temp[i]);
	assertc(Pos == grp->offset + size);
	//disk_seek(esp, grp->offset + size);
}

//void make_top_grup_revisit

api grupp esp_top_grup(cespp esp, const char type[5])
{
	for (unsigned int i = 0; i < esp->grups->size; i++)
	{
	grupp grp = esp->grups->elements[i];
	if (*(unsigned int *)type == grp->hed->label)
	return esp->grups->elements[i];
	}
	return NULL;
}

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
	rcdp rcd = esp->records->elements[i];
	build_form_id(&esp->formIds[i], rcd->hed->formId);
	rcd->form_id = &esp->formIds[i];
	}
}

api rcdp esp_get_form_id(unsigned int formId)
{
	struct form_id form_id;
	build_form_id(&form_id, formId);
	espp esp = get_plugins()[form_id.modIndex];
	if (esp == NULL)
	return NULL;
	for (unsigned int j = 0; j < esp->records->size; j++)
	{
	rcdp rcd = esp->records->elements[j];
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

api revised_array * esp_filter_objects(cespp esp, const char type[5])
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

char *uncompress_record(espp_rcdp)
{
	printf("uncompress\n");
	// char *src = rcd->data;
	// dont use rcd->data, use rcd->offset + sizeof header ?
	char *src = rcd->buf;
	unsigned int realSize = *(unsigned int *)src;
	unsigned int size = rcd->hed->size - 4;
	printf("un size %i\n", size);
	src += 4;
	char *dest = malloc(realSize * sizeof(char));
	int ret = uncompress(dest, (uLongf*)&realSize, src, size);
	assertm(ret == Z_OK, "were zlib'd");
	rcd->size = realSize;
	free(rcd->buf);
	rcd->buf = dest;
	return dest;
}

api esppp get_plugins()
{
	return plugins;
}

api espp has_plugin(const char *name)
{
	for (int i = PLUGINS; i-- > 0;)
	if (plugins[i] != NULL && 0 == strcmp(name, plugins[i]->filename))
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
	rcdp rcd = esp->records->elements[i];
	if (rcd->hed->flags & 0x00040000)
	free(rcd->buf);
	free_esp_array(&rcd->subrecords);
	}
	free_esp_array(&esp->grups);
	free_esp_array(&esp->records);
	free(esp);
}

// Horrible growable c array stolen from GitHub

api void free_esp_array(revised_array ** array)
{
	free((*array)->elements);
}

inline void narray(revised_array ** p, unsigned int initial) {
	*p = malloc(sizeof(struct revised_array));
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

static int disk_read(espp esp, void **data, size_t size)
{
	if (size < 1)
		return 0;
	*data = malloc(size);
	int pos = fread(*data, size, 1, (FILE *)esp->stream);
	Pos += size;
	//assertm(pos == 1, "cant read 1");
	assertc(Pos == disk_tell(esp));
	return pos;
}

static int disk_seek(espp esp, long int offset)
{
	Pos = offset;
	int ok = fseek((FILE *)esp->stream, offset, SEEK_SET);
	return ok;
}

static long int disk_tell(espp esp)
{
	unsigned pos = ftell((FILE *)esp->stream);
	return pos;
}