// see header for stuff

// based on Ortham/libespm

// has some bugs
// will probably crash in some situations
// could do with some rewrite
// read at your own risk

#include <lib/common.h>

#include <lib/esp.h>

#include <zlib.h>
#include <sys/stat.h>

int only_read_editorid = 1;
int skip_field_data = 1;

#if 0
#define spamf(...) if (Buf) printf(__VA_ARGS__);
#else
#define spamf 0;
#endif

static ESP plugins[PLUGINS] = { NULL };

// horrible c vector
inline void narray(revised_array **, unsigned int);
inline void grow(revised_array *);
inline void insert(revised_array *, void *);

static int disk_read(ESP, void **, size_t);
static int disk_seek(ESP, long int);
static long int disk_tell(ESP);

#define Buf esp->buf
#define Pos esp->pos

static void read(ESP esp, void **data, size_t size) {
	if (Buf)
	{
		// we have the whole file,
		// the data is just an offset
		*data = Buf + Pos;
		Pos += size;
	}
	else
		// we have a handle,
		// the data needs to be pulled
		disk_read(esp, data, size);
}

static void seek(ESP esp, long int offset) {
	if (Buf)
		// we have the whole file,
		// our offset is just a number
		Pos = offset;
	else
		// we have a handle,
		// we seek set on the open file
		disk_seek(esp, offset);
}

GRUP read_grp(ESP);
RCD read_rcd(ESP);
SRCD read_rcdb(ESP, RCD);

char *uncompress_record(ESP, RCD);
void loop_rcd(ESP, RCD);
void loop_grup(ESP, GRUP);

unsigned int hedr_num_records(ESP esp)
{
	SRCD hedr = (*(csubrecord ***)esp->header->rcdbs)[0];
	float version = *(float *)(hedr->data);
	unsigned int num_records = *(unsigned int *)(hedr->data + 4);
	// printf("hedr tes4 version %f\n", version);
	return num_records;
}

api ESP plugin_load(const char *path, int whole)
{
	ESP esp = calloc(1, sizeof(struct cesp));
	file_name(esp->filename, path, '/');
	esp->stream = fopen(path, "rb");
	assertm(
		esp->stream, path);
	fseek(esp->stream, 0L, SEEK_END);
	esp->filesize = ftell(esp->stream);
	rewind(esp->stream);
	if (whole) {
		// read the whole file
		disk_read(esp, &Buf, esp->filesize);
		seek(esp, 0);
		fclose(esp->stream);
		//printf("read whole plugin into buf\n");
	}
	spamf("goign to read the esp header\n");
	esp->header = read_rcd(esp);
	esp_check_rcd(esp->header);
	narray(&esp->grups, hedr_num_records(esp));
	narray(&esp->records, 128);
	narray(&esp->large, 10);
	int num = 0;
	while(Pos < esp->filesize)
	{
	GRUP grp = read_grp(esp);
	// if you want everything ready to use, uncomment this
	//loop_grup(esp, grp);
	insert(esp->grups, grp);
	}
	return esp;
}

void skip_rcdb(ESP esp, RCD rcd, size_t);
void read_rcd_buf_or_disk(ESP esp, RCD rcd, void **, size_t);

void read_rcdb_data(ESP esp, RCD rcd, SRCD rcdb);

static void *grp_temp[40000]; // keep this high
static SRCD rcdb_temp[40000]; // keep this high

struct form_id build_form_id(unsigned int);

GRUP read_grp(ESP esp)
{
	spamf("read_grp\n");
	GRUP grp = calloc(1, sizeof(struct cgrup));
	grp->g = 'g';
	grp->id = esp->ids.grups++;
	grp->esp = esp;
	read(esp, &grp->hed, sizeof(struct cgrup_header));
	grp->offset = Pos;
	grp->size = grp->hed->size - sizeof(struct cgrup_header);
	// loop_grup(esp, grp, 1);
	seek(esp, grp->offset + grp->size);
	return grp;
}

RCD read_rcd(ESP esp)
{
	spamf("read rcd\n");

	RCD rcd = calloc(1, sizeof(struct crecord));

	rcd->r = 'r';
	rcd->id = esp->ids.records++;
	rcd->esp = esp;
	read(esp, &rcd->hed, sizeof(struct crecord_header));
	rcd->offset = Pos;
	rcd->size = rcd->hed->size;
	rcd->partial = 1;

	rcd->form_id = build_form_id(rcd->hed->formId);

	insert(esp->records, rcd);

	spamf("printing hed sgn:\n");
	spamf("%.4s %u\n", (char *)&rcd->hed->sgn, rcd->hed->size);

	if (rcd->hed->flags & 0x00040000)
	{
		read(esp, &rcd->buf, rcd->size);
		uncompress_record(esp, rcd);
		loop_rcd(esp, rcd);
	}
	else
		loop_rcd(esp, rcd);

	seek(esp, rcd->offset + rcd->hed->size);
	return rcd;
}

SRCD read_rcdb(ESP esp, RCD rcd)
{
	spamf("read rcdb\n");
	SRCD rcdb = calloc(1, sizeof(struct csubrecord));
	rcdb->s = 's';
	rcdb->id = esp->ids.subrecords++;
	read_rcd_buf_or_disk(esp, rcd, &rcdb->hed, sizeof(struct csubrecord_header));
	rcdb->offset = rcd->buf ? rcd->pos : Pos;
	//read_rcdb_data(esp, rcd, rcdb);
	skip_rcdb(esp, rcd, rcdb->hed->size);
	return rcdb;
}

void read_rcdb_data(ESP esp, RCD rcd, SRCD rcdb)
{
	if (rcdb->data)
		return;
	if (rcd->buf)
		rcd->pos = rcdb->offset;
	else
		seek(esp, rcdb->offset);
	rcdb->data = malloc(rcdb->hed->size);
	read_rcd_buf_or_disk(esp, rcd, &rcdb->data, rcdb->hed->size);
}

unsigned int peek_type(ESP esp)
{
	unsigned int *sgn;
	read(esp, &sgn, sizeof(unsigned int));
	seek(esp, Pos - 4);
	return *sgn;
}

void loop_grup(ESP esp, GRUP grp)
{
	seek(grp->esp, grp->offset);
	
	unsigned start = Pos;
	unsigned int num = 0;
	while (Pos - start < grp->size)
	{
		if (peek_type(esp) == *(unsigned int *)"GRUP")
		{
			GRUP grp2 = read_grp(esp);
			grp_temp[num++] = grp2;
		}
		else
		{
			RCD rcd = read_rcd(esp);
			grp_temp[num++] = rcd;
		}
		assertm(num < 40000, "overflow");
	}
	narray(&grp->mixed, num);
	for (unsigned int i = 0; i < num; i++)
		insert(grp->mixed, grp_temp[i]);
	assertc(Pos == grp->offset + grp->size);
	grp->looped = 1;
}

void loop_rcd(ESP esp, RCD rcd)
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
		SRCD rcdb = read_rcdb(esp, rcd);
		//if (num == 0)
		//read_rcdb_data(esp, rcd, rcdb);

		if (rcdb->hed->sgn == *(unsigned int *)"XXXX")
		{
			// printf("oef");
			read_rcdb_data(esp, rcd, rcdb);
			SRCD discard = read_rcdb(esp, rcd);
			if (rcd->buf)
				printf("\ncompressed oversized sub!!\n\n");
			skip_rcdb(esp, rcd, *(unsigned int *)rcdb->data);
			continue;
		}
		rcdb_temp[num++] = rcdb;
	}
	narray(&rcd->rcdbs, num);
	for (unsigned int i = 0; i < num; i++)
		insert(rcd->rcdbs, rcdb_temp[i]);
}

api void esp_check_grup(GRUP grp)
{
	if (grp==NULL)
		return;
	if (grp->g == 'r') {
		printf("not a g but an r!\n");
	}
	assertc(grp->g == 'g');
	if (!grp->looped)
		loop_grup(grp->esp, grp);
}

api void esp_check_rcd(RCD rcd)
{
	if (!rcd)
		return;
	assertc(rcd->r == 'r');
	if (rcd->partial)
	{
		//printf("checked partial rcd\n");
		for (unsigned int i = 0; i < rcd->rcdbs->size; i++)
		{
			SRCD rcdb = rcd->rcdbs->elements[i];
			if (rcdb->data)
				continue;
			if (rcd->buf)
				rcd->pos = rcdb->offset;
			else
				seek(rcd->esp, rcdb->offset);
			read_rcdb_data(rcd->esp, rcd, rcdb);
		}
		rcd->partial = 0;
	}
}

void read_rcd_buf_or_disk(ESP esp, RCD rcd, void **dest, size_t size)
{
	if (rcd->buf)
		*dest = rcd->buf + rcd->pos;
	else
		read(esp, dest, size);
	rcd->pos += size;
}

void skip_rcdb(ESP esp, RCD rcd, size_t n)
{
	// printf("skip %u\n", n);
	if (!rcd->buf)
	{
		Pos += n;
		seek(esp, Pos);
	}
	rcd->pos += n;
}

//void make_top_grup_revisit

api GRUP esp_top(const ESP esp, const char type[5])
{
	if (esp==NULL)
		return NULL;
	for (unsigned int i = 0; i < esp->grups->size; i++)
	{
	GRUP grp = esp->grups->elements[i];
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

// We do not map the TES4 -> MASTers
// For my mod, this is fine
api RCD esp_get_form_id(unsigned int formId)
{
	struct form_id form_id;
	form_id = build_form_id(formId);
	ESP esp = get_plugins()[form_id.modIndex];
	// printf("get modIndex %u\n", form_id.modIndex);
	if (esp == NULL)
	return NULL;
	for (unsigned int j = 0; j < esp->records->size; j++)
	{
	RCD rcd = esp->records->elements[j];
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

api revised_array *esp_filter_objects(const ESP esp, const char type[5])
{
	revised_array *filtered;
	narray(&filtered, 100);
	for (unsigned int i = 0; i < esp->records->size; i++)
	{
	RCD record = esp->records->elements[i];
	if (record->hed->sgn == *(unsigned int *)type)
	insert(filtered, record);
	}
	return filtered;
}

char *uncompress_record(ESP esp, RCD rcd)
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
	if (!Buf)
		// free the whole file
		free(rcd->buf);
	rcd->buf = dest;
	return dest;
}

api ESP *get_plugins()
{
	return plugins;
}

api ESP has_plugin(const char *name)
{
	for (int i = PLUGINS; i-- > 0;)
	if (plugins[i] != NULL && 0 == strcmp(name, plugins[i]->filename))
	return plugins[i];
	return NULL;
}

api void free_plugin(ESP *p)
{
	ESP esp = *p;
	*p = NULL;
	if (esp == NULL)
	return;
	printf("free plugin\n");
	for (int i = PLUGINS; i --> 0; )
	if (esp == plugins[i])
	plugins[i] = NULL;
	for (unsigned int i = 0; i < esp->records->size; i++)
	{
	RCD rcd = esp->records->elements[i];
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

static int disk_read(ESP esp, void **data, size_t size)
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

static int disk_seek(ESP esp, long int offset)
{
	Pos = offset;
	int ok = fseek((FILE *)esp->stream, offset, SEEK_SET);
	return ok;
}

static long int disk_tell(ESP esp)
{
	unsigned pos = ftell((FILE *)esp->stream);
	return pos;
}