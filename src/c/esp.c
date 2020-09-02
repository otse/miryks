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

static int read(struct esp *, void *, size_t, size_t);
static int seek(struct esp *, unsigned int);
static int skip(struct esp *, unsigned int);
static unsigned int tell(struct esp *);

struct record *read_record(struct esp *, Parent);
void read_record_subrecords(struct esp *, struct record *);
struct subrecord *read_subrecord(struct esp *, Parent, unsigned int);
struct grup *read_grup(struct esp *, Parent);
void read_grup_records(struct esp *, Parent);

void read_subrecord_data(struct esp *, struct subrecord *);

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
	unsigned int pos = tell(esp);
	while(tell(esp) < esp->filesize)
	{
	read_grup(esp, 0);
	}
	return esp;
}

struct record *read_record(struct esp *esp, Parent parent) {
	struct record *rec = malloc(sizeof(struct record));
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
	skip(esp, rec->dataSize);
	else
	read_record_subrecords(esp, rec);
	return rec;
}
void report_xxxx(struct esp *esp, struct subrecord *sub)
{
	printf("\nXXXX %u\n", tell(esp));
}
void read_record_subrecords(struct esp *esp, struct record *rec) {
	unsigned int pos = tell(esp);
	unsigned int override = 0;
	while(tell(esp) - pos < rec->dataSize)
	{
	//printf(".");
	struct subrecord *sub;
	sub = read_subrecord(esp, rec->parent, override);
	if (sub->type == XXXX)
	{
	report_xxxx(esp, sub);
	override = *(unsigned int *)sub->buf;
	}
	else
	override = 0;
	}
	return rec;
}

struct subrecord *read_subrecord(struct esp *esp, Parent parent, unsigned int override) {
	struct subrecord *sub = malloc(sizeof(struct subrecord));
	memset(sub, 0, sizeof(struct subrecord));
	sub->x = SUBRECORD;
	sub->id = Subrecords++;
	read(esp, &sub->type, 4, 1);
	if (override)
	{
	sub->size = override;
	skip(esp, 2);
	}
	else
	read(esp, &sub->size, 2, 1);
	#if 1
	read_subrecord_data(esp, sub);
	#else
	int pos = ftell(esp->stream);
	rec->start = pos;
	seek(esp, pos + rec->length);
	#endif
	//printf("S %s %u > ", sub->type, sub->size);
	return sub;
}

void read_subrecord_data(struct esp *esp, struct subrecord *sub) {
	sub->buf = malloc(sizeof(char) * sub->size);
	read(esp, sub->buf, 1, sub->size);
	//skip(esp, sub->size);
}

void peek_type(struct esp *esp, int *type)
{
	unsigned int pos = tell(esp);
	read(esp, type, 4, 1);
	seek(esp, pos);
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
	report_group(esp, grup);
	// records
	read_grup_records(esp, grup);
	return grup;
}
void read_grup_records(struct esp *esp, struct grup *grup) {
	int size = grup->size - 4 - 4 - 16;
	unsigned int pos = ftell(esp->stream);
	int start = pos;
	while (start - pos < size)
	{
	int type;
	peek_type(esp, &type);
	//printf("peek type %s ", type);
	if (type == GRUP)
	{
	//printf("peeked grup\n");
	read_grup(esp, grup->parent);
	}
	else
	{
	read_record(esp, grup->parent);
	}
	pos = ftell(esp->stream);
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

static int read(struct esp *esp, void *data, size_t size, size_t count)
{
	return fread(data, size, count, esp->stream);
}

static int seek(struct esp *esp, unsigned int offset)
{
	return fseek(esp->stream, offset, SEEK_SET);
}

static int skip(struct esp *esp, unsigned int offset)
{
	return fseek(esp->stream, offset, SEEK_CUR);
}

static unsigned int tell(struct esp *esp)
{
	return ftell(esp->stream);
}

int report_at = 10;

void report_load_percentage(struct esp *esp)
{
	unsigned int pos = ((float)tell(esp) / esp->filesize) * 100;
	if (pos >= report_at)
	{
		printf("\n\n%u%% loaded\n\n", pos);
		report_at += 10;
		Sleep(300);
	}
}