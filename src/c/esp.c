// based on ortham/libespm

#include "c/c.h"

#include "esp.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#define Pos rec->pos

typedef int Parent;

static inline int read(struct esp *, void *, size_t, size_t);
static inline int seek(struct esp *, long);
static inline int skip(struct esp *, unsigned int);
static long int tell(struct esp *);

struct record *read_record(struct esp *, Parent);
void read_record_subrecords(struct esp *, struct record *);
struct subrecord *read_subrecord(struct esp *, Parent, unsigned int);
struct grup *read_grup(struct esp *, Parent);
void read_grup_records(struct esp *, Parent);

void read_subrecord_into_buf(struct esp *, struct subrecord *);

void report_load_percentage(struct esp *);

#define GRUP 0x50555247
#define XXXX 0x58585858

int Grups = 0;
int Records = 0;
int Subrecords = 0;

api struct esp *esp_load(const char *path) {
	printf("esp load\n");
	struct esp *esp = malloc(sizeof(struct esp));
	memset(esp, 0, sizeof(struct esp));
	esp->path = malloc(sizeof(char) * strlen(path) + 1);
	strcpy(esp->path, path);
	esp->stream = fopen(path, "rb");
	fseek(esp->stream, 0, SEEK_END);
	esp->filesize = ftell(esp->stream);
	fseek(esp->stream, 0, SEEK_SET);
	seek(esp, 0);
	cassert_(
		esp->stream, "esp can't open");
	esp->header = read_record(esp, 0);
	long int pos = tell(esp);
	while(tell(esp) < esp->filesize)
	{
	read_grup(esp, 0);
	}
	return esp;
}

struct record *read_record(struct esp *esp, Parent parent) {
	struct record *rec;
	rec = malloc(sizeof(struct record));
	memset(rec, 0, sizeof(struct record));
	//  header
	rec->x = RECORD;
	rec->id = Records++;
	read(esp, &rec->type, 4, 1);
	read(esp, &rec->dataSize, 4, 1);
	read(esp, &rec->flags, 4, 1);
	read(esp, &rec->formId, 4, 1);
	skip(esp, 8);
	//printf("R %s %u > ", rec->type, rec->dataSize);
	// subrecords
	if ((rec->flags & 0x00040000) != 0)
	{
	skip(esp, rec->dataSize);
	}
	else
	read_record_subrecords(esp, rec);
	return rec;
}
void report_xxxx(struct esp *esp, struct subrecord *sub, unsigned int override)
{
	printf("\nXXXX at %u size %u\n", tell(esp), override);
}
unsigned int large_landmark(struct esp *esp, struct subrecord *sub)
{
	if (sub->type == XXXX)
	{
	read_subrecord_into_buf(esp, sub);
	unsigned int large = *(unsigned int *)sub->buf;
	report_xxxx(esp, sub, large);
	return large;
	}
	return 0;
}
void read_record_subrecords(struct esp *esp, struct record *rec) {
	long int pos = tell(esp);
	unsigned int large = 0;
	while(tell(esp) - pos < rec->dataSize)
	{
	struct subrecord *sub;
	sub = read_subrecord(esp, rec->parent, large);
	large = large_landmark(esp, sub);
	}
	return rec;
}

struct subrecord *read_subrecord(struct esp *esp, Parent parent, unsigned int override) {
	struct subrecord *sub;
	sub = malloc(sizeof(struct subrecord));
	memset(sub, 0, sizeof(struct subrecord));
	sub->x = SUBRECORD;
	sub->id = Subrecords++;
	read(esp, &sub->type, 4, 1);
	if (override == 0)
	read(esp, (unsigned short *)&sub->size, 2, 1);
	else
	{
	sub->size = override;
	skip(esp, 2);
	}
	#if 1
	read_subrecord_into_buf(esp, sub);
	#else
	sub->offset = tell(esp);
	skip(esp, sub->size);
	#endif
	//printf("S %s %u > ", sub->type, sub->size);
	return sub;
}

void read_subrecord_into_buf(struct esp *esp, struct subrecord *sub)
{
	if (sub->buf != NULL)
	return;
	long late = sub->offset ? tell(esp) : 0;
	if (late)
	seek(esp, sub->offset);
	sub->buf = malloc(sizeof(char) * sub->size);
	read(esp, sub->buf, 1, sub->size);
	if (late)
	seek(esp, late);
}

const unsigned int peek_type(struct esp *esp)
{
	unsigned int type;
	long int pos = tell(esp);
	read(esp, &type, 4, 1);
	seek(esp, pos);
	return type;
}
void report_group(struct esp *esp, struct grup *grup)
{
	printf("G %s %u > ", (char *)&grup->type, grup->size);
}
struct grup *read_grup(struct esp *esp, Parent parent) {
	report_load_percentage(esp);
	struct grup *grup = malloc(sizeof(struct grup));
	memset(grup, 0, sizeof(struct grup));
	// header
	grup->x = GRUP;
	grup->id = Grups++;
	grup->parent = parent;
	read(esp, &grup->type, 4, 1);
	read(esp, &grup->size, 4, 1);
	skip(esp, 16);
	// report_group(esp, grup);
	// records
	read_grup_records(esp, grup);
	return grup;
}
void read_grup_records(struct esp *esp, struct grup *grup) {
	long size = grup->size - 4 - 4 - 16;
	long pos = tell(esp);
	long start = pos;
	while (start - pos < size)
	{
	//printf("peek type %s ", type);
	if (peek_type(esp) == GRUP)
	{
	//printf("peeked grup\n");
	read_grup(esp, grup->parent);
	}
	else
	{
	read_record(esp, grup->parent);
	}
	pos = tell(esp);
	}
}

api void esp_free(struct esp **p)
{
	struct esp *esp = *p;
	*p = NULL;
	fclose(esp->stream);
	free(esp);
	return;
}

static inline int read(struct esp *esp, void *data, size_t size, size_t count)
{
	return fread(data, size, count, esp->stream);
}

static inline int seek(struct esp *esp, long offset)
{
	return fseek(esp->stream, offset, SEEK_SET);
}

static inline int skip(struct esp *esp, unsigned int offset)
{
	return fseek(esp->stream, offset, SEEK_CUR);
}

static inline long tell(struct esp *esp)
{
	return ftell(esp->stream);
}

int report_at = 10;

void report_load_percentage(struct esp *esp)
{
	long int pos = ((double)tell(esp) / esp->filesize) * 100;
	if (pos >= report_at)
	{
		printf("\n\n%u%% loaded\n\n", pos);
		report_at += 10;
		//Sleep(300);
	}
}