// based on ortham/libespm

#include "c/c.h"

#include "esp.h"

#include <time.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

static inline void read(struct esp *, void *, size_t, size_t);

struct record *read_record(struct esp *);
void read_record_subrecords(struct esp *, struct record *);
struct subrecord *read_subrecord(struct esp *, unsigned int);
struct grup *read_grup(struct esp *);
void read_grup_records(struct esp *, struct grup *);

void read_subrecord_into_buf(struct esp *, struct subrecord *);

void report_load_percentage(struct esp *);

#define GRUP 0x50555247
#define XXXX 0x58585858

#define Buf    esp->buf
#define Pos    esp->pos

#define Depos  Buf + Pos
#define FromBuf(x) *(x*)(Depos); Pos += sizeof(x);

int report = 0;
int esp_skip_subrecords = 0;

int Grups = 0;
int Records = 0;
int Subrecords = 0;

float read_entire_file(struct esp *esp)
{
	clock_t before, after;
	before = clock();
	fseek(esp->file, 0, SEEK_END);
	esp->filesize = ftell(esp->file);
	rewind(esp->file);
	esp->buf = malloc(sizeof(char) * esp->filesize);
	size_t read = fread(esp->buf, sizeof(char), esp->filesize, esp->file);
	cassert_(read, "esp buf bad");
	rewind(esp->file);
	after = clock();
	float difference = (float)(after - before) / CLOCKS_PER_SEC;
	printf("read entire esp %i mb took %.2fs\n", esp->filesize / 1024 / 1024, difference);
	return difference;
}
api struct esp *esp_load(const char *path)
{
	printf("esp load\n");
	clock_t before, after;
	float entirety;
	struct esp *esp = malloc(sizeof(struct esp));
	memset(esp, 0, sizeof(struct esp));
	esp->path = malloc(sizeof(char) * strlen(path) + 1);
	strcpy(esp->path, path);
	esp->file = fopen(path, "rb");
	cassert_(esp->file, "esp can't open");
	entirety = read_entire_file(esp);
	Pos = 0;
	before = clock();
	esp->header = read_record(esp);
	while(Pos < esp->filesize)
	{
	read_grup(esp);
	}
	after = clock();
	float difference = (float)(after - before) / CLOCKS_PER_SEC;
	printf("parsing esp took %.2fs, alltogether %.2fs\n", difference, difference + entirety);
	printf("esp has %i grups %i records and %i subrecords\n", Grups, Records, Subrecords);
	return esp;
}

void no_hog(struct esp *esp) {

}

void report_record(struct record *rec)
{
	if (!report)
	return;
	printf("R %.4s %u > ", (char *)&rec->head->type, rec->head->dataSize);
}
struct record *read_record(struct esp *esp)
{
	struct record *rec;
	rec = malloc(sizeof(struct record));
	memset(rec, 0, sizeof(struct record));
	{
	Records++;
	rec->head = Depos;
	Pos += sizeof(struct record_head);
	Pos += 8;
	}
	report_record(rec);
	// subrecords
	if (esp_skip_subrecords || (rec->head->flags & 0x00040000) != 0)
	Pos += rec->head->dataSize;
	else
	read_record_subrecords(esp, rec);
	return rec;
}

void report_xxxx(struct esp *esp, struct subrecord *sub, unsigned int override)
{
	printf("\nXXXX at %u size %u\n", Pos, override);
}
unsigned int large_landmark(struct esp *esp, struct subrecord *sub)
{
	if (sub->head->type == XXXX)
	{
	read_subrecord_into_buf(esp, sub);
	unsigned int large_sub = *(unsigned int *)sub->data;
	report_xxxx(esp, sub, large_sub);
	return large_sub;
	}
	return 0;
}
void read_record_subrecords(struct esp *esp, struct record *rec)
{
	long pos = Pos;
	unsigned int large_sub = 0;
	while(Pos - pos < (long)rec->head->dataSize)
	{
	struct subrecord *sub;
	sub = read_subrecord(esp, large_sub);
	large_sub = large_landmark(esp, sub);
	}
}

void report_subrecord(struct subrecord *sub)
{
	if (!report)
	return;
	printf("S %.4s %u > ", (char *)&sub->head->type, sub->head->size);
}
struct subrecord *read_subrecord(struct esp *esp, unsigned int override)
{
	struct subrecord *sub;
	sub = malloc(sizeof(struct subrecord));
	memset(sub, 0, sizeof(struct subrecord));
	{
	Subrecords++;
	sub->head = Depos;
	Pos += sizeof(struct subrecord_head);
	}
	if (override)
	{
	sub->size = override;
	Pos += 2;
	}
	else
	sub->size = sub->head->size;
	sub->data = Depos;
	Pos += sub->size;
	report_subrecord(sub);
	return sub;
}

void read_subrecord_into_buf(struct esp *esp, struct subrecord *sub)
{
	
}

void report_group(struct esp *esp, struct grup *grup)
{
	if (!report)
	return;
	printf("G %.4s %u > ", (char *)&grup->head->type, grup->head->size);
}
struct grup *read_grup(struct esp *esp)
{
	report_load_percentage(esp);
	struct grup *grup = malloc(sizeof(struct grup));
	memset(grup, 0, sizeof(struct grup));
	// header
	{
	// grup->x = GRUP;
	Grups++;
	grup->head = Depos;
	Pos += sizeof(struct grup_head);
	Pos += 16;
	}
	report_group(esp, grup);
	// records
	read_grup_records(esp, grup);
	// printf("\nend grup\n");
	return grup;
}
const unsigned int peek_type(struct esp *esp)
{
	return *(unsigned int *)(Depos);
}
void read_grup_records(struct esp *esp, struct grup *grup)
{
	long size = grup->head->size - 4 - 4 - 16;
	long start = Pos;
	while (Pos - start < size)
	{
	if (peek_type(esp) == GRUP)
	read_grup(esp);
	else
	read_record(esp);
	}
}

api void esp_free(struct esp **p)
{
	struct esp *esp = *p;
	*p = NULL;
	fclose(esp->file);
	free(esp);
	return;
}

static inline void read(struct esp *esp, void *dest, size_t size, size_t count)
{
	memcpy(dest, Depos, size * count);
	Pos += size * count;
}

int load_tens = 10;

void report_load_percentage(struct esp *esp)
{
	long pos = ((double)Pos / esp->filesize) * 100;
	if (pos >= load_tens)
	{
	printf("%u%% loaded\n", pos);
	load_tens += 10;
	//if (pos >= 80)
	//report = 1;;
	}
}