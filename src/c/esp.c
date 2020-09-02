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
static inline void seek(struct esp *, long);
static inline void skip(struct esp *, unsigned int);

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

unsigned char *read_entire_file(struct esp *esp)
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
}
api struct esp *esp_load(const char *path)
{
	printf("esp load\n");
	clock_t before, after;
	struct esp *esp = malloc(sizeof(struct esp));
	memset(esp, 0, sizeof(struct esp));
	esp->path = malloc(sizeof(char) * strlen(path) + 1);
	strcpy(esp->path, path);
	esp->file = fopen(path, "rb");
	cassert_(esp->file, "esp can't open");
	read_entire_file(esp);
	Pos = 0;
	before = clock();
	esp->header = read_record(esp);
	while(Pos < esp->filesize)
	{
	read_grup(esp, 0);
	}
	after = clock();
	float difference = (float)(after - before) / CLOCKS_PER_SEC;
	printf("done parsing esp took %.2fs\n", difference);
	printf("esp has %i grups %i records and %i subrecords\n", Grups, Records, Subrecords);
	return esp;
}

void no_hog(struct esp *esp) {

}

void report_record(struct record *rec)
{
	if (report)
	printf("R %s %u > ", (char *)&rec->type, rec->dataSize);
}
struct record *read_record(struct esp *esp)
{
	struct record *rec;
	rec = malloc(sizeof(struct record));
	memset(rec, 0, sizeof(struct record));
	//  header
	{
	// rec->x = RECORD;
	rec->id = Records++;
	read(esp, rec, 4 + 4 + 4 + 4, 1);
	skip(esp, 8);
	}
	report_record(rec);
	// subrecords
	if (esp_skip_subrecords || (rec->flags & 0x00040000) != 0)
	skip(esp, rec->dataSize);
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
	if (sub->type == XXXX)
	{
	read_subrecord_into_buf(esp, sub);
	unsigned int large_sub = *(unsigned int *)sub->buf;
	report_xxxx(esp, sub, large_sub);
	return large_sub;
	}
	return 0;
}
void read_record_subrecords(struct esp *esp, struct record *rec)
{
	long pos = Pos;
	unsigned int large_sub = 0;
	while(Pos - pos < (long)rec->dataSize)
	{
	struct subrecord *sub;
	sub = read_subrecord(esp, large_sub);
	large_sub = large_landmark(esp, sub);
	}
}

void report_subrecord(struct subrecord *sub)
{
	if (report)
	printf("S %s %u > ", (char *)&sub->type, sub->size);
}
struct subrecord *read_subrecord(struct esp *esp, unsigned int override)
{
	struct subrecord *sub;
	sub = malloc(sizeof(struct subrecord));
	memset(sub, 0, sizeof(struct subrecord));
	{
	// sub->x = SUBRECORD;
	sub->id = Subrecords++;
	read(esp, sub, 4 + 2, 1);
	}
	//read(esp, (unsigned short *)&sub->size, 2, 1);
	if (override)
	{
	sub->size = override;
	skip(esp, 2);
	}
	#if 0
	read_subrecord_into_buf(esp, sub);
	#else
	sub->offset = Pos;
	skip(esp, sub->size);
	#endif
	// report_subrecord(sub);
	return sub;
}

void read_subrecord_into_buf(struct esp *esp, struct subrecord *sub)
{
	if (sub->buf != NULL)
	return;
	long late = sub->offset ? Pos : 0;
	if (late)
	seek(esp, sub->offset);
	sub->buf = malloc(sizeof(char) * sub->size);
	read(esp, sub->buf, sub->size, 1);
	if (late)
	seek(esp, late);
}

void report_group(struct esp *esp, struct grup *grup)
{
	if (report)
	printf("G %s %u > ", (char *)&grup->type, grup->size);
}
struct grup *read_grup(struct esp *esp)
{
	report_load_percentage(esp);
	struct grup *grup = malloc(sizeof(struct grup));
	memset(grup, 0, sizeof(struct grup));
	// header
	{
	// grup->x = GRUP;
	grup->id = Grups++;
	read(esp, &grup->type, 4, 1);
	read(esp, &grup->size, 4, 1);
	skip(esp, 16);
	}
	report_group(esp, grup);
	// records
	read_grup_records(esp, grup);
	// printf("\nend grup\n");
	return grup;
}
const unsigned int peek_type(struct esp *esp)
{
	unsigned int type;
	long pos = Pos;
	read(esp, &type, 4, 1);
	seek(esp, pos);
	return type;
}
void read_grup_records(struct esp *esp, struct grup *grup)
{
	long size = grup->size - 4 - 4 - 16;
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
static inline void seek(struct esp *esp, long offset)
{
	Pos = offset;
}
static inline void skip(struct esp *esp, unsigned int offset)
{
	Pos += offset;
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