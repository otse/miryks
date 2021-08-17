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

//void make_form_ids(espp);

rcdp read_record(espp, int);
grupp read_grup(espp, grupp, int);

unsigned int hedr_num_records(espp esp)
{
	rcdbp hedr = (*(subrecord ***)esp->header->rcdbs)[0];
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
	esp->header = read_record(esp, 0);
	narray(&esp->grups, hedr_num_records(esp));
	narray(&esp->records, 400);
	narray(&esp->large, 10);
	int num = 0;
	while(Pos < esp->filesize)
	{
	grupp grp = read_grup(esp, NULL, 1);
	insert(esp->grups, grp);
	}
	spamf("last\n");
	esp->override = 1;
	return esp;
}


char *uncompress_record(espp, rcdp);
void loop_rcd(espp, rcdp, int);
void loop_grup(espp, grupp, int);

struct form_id build_form_id(unsigned int);

rcdp read_record(espp esp, int fast)
{
	spamf("read rcd\n");

	rcdp rcd = calloc(1, sizeof(struct record));

	rcd->r = 'r';
	rcd->id = esp->ids.records++;
	rcd->esp = esp;
	disk_read(esp, &rcd->hed, sizeof(struct record_header));
	rcd->offset = Pos;
	rcd->size = rcd->hed->size;

	rcd->form_id = build_form_id(rcd->hed->formId);

	insert(esp->records, rcd);

	spamf("rcd - sgn, size %.4s %u\n", (char *)&rcd->hed->sgn, rcd->hed->size);
	if (rcd->hed->flags & 0x00040000)
	{
		//if (!fast)
		{
			//printf("rcd %.4s is compressed\n", (char *)&rcd->hed->sgn);
			
			disk_read(esp, &rcd->buf, rcd->size);
			uncompress_record(esp, rcd);
			//printf("real size %u\n", rcd->size);
			loop_rcd(esp, rcd, 0);
			//skip()
			
			//disk_seek(esp, rcd->offset + rcd->size);
		}
	}
	else
		loop_rcd(esp, rcd, fast);
	disk_seek(esp, rcd->offset + rcd->hed->size);
	return rcd;
}

#define espp_rcdp espp esp, rcdp rcd

void skip(espp_rcdp, size_t);
void read(espp_rcdp, rcdbp, void **, size_t);

rcdbp read_rcdb(espp_rcdp, int);
rcdbp read_rcdb_data(espp_rcdp, rcdbp, int);

static rcdbp rcdb_temp[40000]; // keep this high

void loop_rcd(espp_rcdp, int fast)
{
	assertc(rcd->buf || Pos == rcd->offset);

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
		rcdbp rcdb = read_rcdb(esp, rcd, 0);
		if (rcdb->hed->sgn == *(unsigned int *)"XXXX")
		{
			//disk_seek(esp, rcdb->offset);
			read_rcdb_data(esp, rcd, rcdb, 0);
			printf("\nuh no\n\n");
			rcdbp discard = read_rcdb(esp, rcd, 1);
			if (rcd->buf)
				printf("\ncompressed oversized sub!!\n\n");
			skip(esp, rcd, *(unsigned int *)rcdb->data);
			continue;
		}
		rcdb_temp[num++] = rcdb;
	}

	narray(&rcd->rcdbs, num);
	for (unsigned int i = 0; i < num; i++)
		insert(rcd->rcdbs, rcdb_temp[i]);
	rcd->looped = 1;
}

api void esp_check_grup(grupp grp)
{
	if (!grp->looped)
	{
		//printf("esp_check_grup: type %i !\n", grp->hed->group_type);
		disk_seek(grp->esp, grp->offset);
		loop_grup(grp->esp, grp, 1);
	}
}

api void esp_check_rcd(rcdp rcd)
{
	if (!rcd->looped)
	{
		printf("rcd %.4s is unlooped\n", (char *)&rcd->hed->sgn);
		if (rcd->buf)
			rcd->pos = 0;
		disk_seek(rcd->esp, rcd->offset);
		loop_rcd(rcd->esp, rcd, 0);
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

rcdbp read_rcdb(espp_rcdp, int nah)
{
	// printf("read rcdb\n");
	rcdbp rcdb = calloc(1, sizeof(struct subrecord));
	rcdb->s = 's';
	rcdb->id = esp->ids.subrecords++;
	read(esp, rcd, &rcdb->hed, sizeof(struct subrecord_header));
	//if (rcd->buf)
	//printf("rcdb - sgn, size %.4s %hu\n", (char *)&rcdb->hed->sgn, rcdb->hed->size);
	rcdb->offset = rcd->buf ? rcd->pos : Pos;
	read_rcdb_data(esp, rcd, rcdb, nah);
#if 0
	if (rcdb->hed->sgn == *(unsigned int *)"MAST")
		printf("MAST %s\n", (char *)rcdb->data);
#endif
	return rcdb;
}

rcdbp read_rcdb_data(espp_rcdp, rcdbp rcdb, int nah)
{
	if (nah)
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


grupp read_grup(espp esp, grupp parent, int fast)
{
	spamf("read_grup\n");
	grupp grp = calloc(1, sizeof(struct grup));
	grp->g = 'g';
	grp->id = esp->ids.grups++;
	grp->esp = esp;
	disk_read(esp, &grp->hed, sizeof(struct grup_header));
	if (!parent)
		spamf("top grp %u %.4s\n", esp->grups->size, (char *)&grp->hed->label);
	grp->offset = Pos;
	grp->size = grp->hed->size - sizeof(struct grup_header);
	if (fast /*&& grp->hed->group_type >= 2 && grp->hed->group_type <= 5*/)
	{
		disk_seek(esp, grp->offset + grp->size);
		return grp;
	}
	loop_grup(esp, grp, fast);
	return grp;
}

unsigned int peek_type(espp esp)
{
	unsigned int *sgn;
	disk_read(esp, &sgn, sizeof(unsigned int));
	disk_seek(esp, Pos - 4);
	return *sgn;
}

static void *grp_temp[40000]; // keep this high

void loop_grup(espp esp, grupp grp, int fast)
{
	assertc(esp->pos == grp->offset);
	
	unsigned start = Pos;
	unsigned int num = 0;
	while (Pos - start < grp->size)
	{
	assertm(num < 40000, "overflow");
	if (peek_type(esp) == *(unsigned int *)"GRUP")
	{
		grupp grp2 = read_grup(esp, grp, fast);
		grp_temp[num++] = grp2;
	}
	else
	{
		rcdp rcd = read_record(esp, fast);
		grp_temp[num++] = rcd;
	}
	}
	narray(&grp->mixed, num);
	for (unsigned int i = 0; i < num; i++)
		insert(grp->mixed, grp_temp[i]);
	if (Pos != grp->offset + grp->size)
	{
		printf("expected Pos == %u, got %u\n", grp->offset + grp->size, Pos);
	}
	assertc(Pos == grp->offset + grp->size);
	if (grp->looped)
	{
		printf("looped an unlooped grup, mixed %i, size %u\n", grp->mixed, grp->mixed->size);
	}
	grp->looped = 1;
	//disk_seek(esp, grp->offset + size);
}

//void make_top_grup_revisit

api grupp esp_top_grup(cespp esp, const char type[5])
{
	if (esp==NULL)
		return NULL;
	for (unsigned int i = 0; i < esp->grups->size; i++)
	{
	grupp grp = esp->grups->elements[i];
	if (*(unsigned int *)type == grp->hed->label)
	return esp->grups->elements[i];
	}
	return NULL;
}

inline struct form_id build_form_id(unsigned int formId)
{
	struct form_id form_id;
	form_id.formId = formId;
	form_id.modIndex = formId >> 24;
	form_id.objectIndex = formId & ~(form_id.modIndex << 24);
	return form_id;
}

api rcdp esp_get_form_id(unsigned int formId)
{
	struct form_id form_id;
	form_id = build_form_id(formId);
	espp esp = get_plugins()[form_id.modIndex];
	if (esp == NULL)
	return NULL;
	for (unsigned int j = 0; j < esp->records->size; j++)
	{
	rcdp rcd = esp->records->elements[j];
	if (rcd->form_id.objectIndex == form_id.objectIndex)
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

api revised_array *esp_filter_objects(cespp esp, const char type[5])
{
	revised_array *filtered;
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
	//printf("uncompress\n");
	// char *src = rcd->data;
	// dont use rcd->data, use rcd->offset + sizeof header ?
	char *src = rcd->buf;
	unsigned int realSize = *(unsigned int *)src;
	unsigned int size = rcd->hed->size - 4;
	//printf("un size %i\n", size);
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
	free_esp_array(&rcd->rcdbs);
	}
	free_esp_array(&esp->grups);
	free_esp_array(&esp->records);
	free(esp);
}

// Horrible growable c array stolen from GitHub

api void free_esp_array(revised_array ** array)
{
	//for (int i = (*array)->size; i --> 0; )
	//	free()
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