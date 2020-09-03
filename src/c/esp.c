// based on ortham/libespm

#include "c/c.h"

#include "esp.h"

#include <time.h>


static inline void copy(struct esp *, void *, size_t, size_t);

struct record *read_record(struct esp *);
void read_record_subrecords(struct esp *, struct record *);
struct subrecord *read_subrecord(struct esp *, unsigned int);
struct grup *read_grup(struct esp *);
void read_grup_records(struct esp *, struct grup *);

void read_subrecord_into_buf(struct esp *, struct subrecord *);

#define Buf esp->buf
#define Pos esp->pos
#define Depos Buf + Pos

int report = 0;
int esp_skip_subrecords = 0;

unsigned int grups = 0;
unsigned int records = 0;
unsigned int subrecords = 0;

inline void array(struct esp_array *a, size_t initial) {
	a->size = initial;
	a->used = 0;
	a->array = malloc(a->size * sizeof(void **));
}

inline void grow(struct esp_array *a) {
	if (a->used != a->size)
	return;
	a->size *= 2;
	a->array = realloc(a->array, a->size * sizeof(void **));
}

inline void insert(struct esp_array *a, void *element) {
	grow(a);
	a->array[a->used++] = element;
}

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
	array(&esp->grups, 200);
	array(&esp->statics, 200);
	//array(&esp->records, 200);
	//array(&esp->subrecords, 200);
	while(Pos < esp->filesize)
	{
	void *grup = read_grup(esp);
	insert(&esp->grups, grup);
	}
	after = clock();
	float difference = (float)(after - before) / CLOCKS_PER_SEC;
	printf("parsing esp took %.2fs, alltogether %.2fs\n", difference, difference + entirety);
	printf("esp has %i grups %i records %i subrecords\n", grups, records, subrecords);
	return esp;
}

struct record *read_record(struct esp *esp)
{
	struct record *rec;
	rec = malloc(sizeof(struct record));
	// head
	rec->x = RECORD;
	rec->id = records++;
	rec->head = Depos;
	Pos += sizeof(struct record_head);
	Pos += 8;
	array(&rec->subrecords, 6);
	if (rec->head->type == ESP_STAT_HEX)
	insert(&esp->statics, rec);
	// printf("R %.4s %u > ", (char *)&rec->head->type, rec->head->dataSize);
	// subrecords
	if (esp_skip_subrecords || (rec->head->flags & 0x00040000) != 0)
	Pos += rec->head->dataSize;
	else
	read_record_subrecords(esp, rec);
	return rec;
}

void read_record_subrecords(struct esp *esp, struct record *rec)
{
	long pos = Pos;
	unsigned int large = 0;
	while(Pos - pos < (long)rec->head->dataSize)
	{
	struct subrecord *sub;
	sub = read_subrecord(esp, large);
	large = 0;
	if (sub->head->type == ESP_XXXX_HEX)
	large = *(unsigned int *)sub->data;
	//insert(&esp->subrecords, sub);
	insert(&rec->subrecords, sub);
	}
}

struct subrecord *read_subrecord(struct esp *esp, unsigned int override)
{
	struct subrecord *sub;
	sub = malloc(sizeof(struct subrecord));
	// head
	sub->x = SUBRECORD;
	sub->id = subrecords++;
	sub->head = Depos;
	Pos += sizeof(struct subrecord_head);
	if (override)
	sub->actualSize = override;
	else
	sub->actualSize = sub->head->size;
	// data
	sub->data = Depos;
	Pos += sub->actualSize;
	// printf("S %.4s %u > ", (char *)&sub->head->type, sub->head->size);
	return sub;
}

struct grup *read_grup(struct esp *esp)
{
	struct grup *grup = malloc(sizeof(struct grup));
	// head
	grup->x = GRUP;
	grup->id = grups++;
	grup->head = Depos;
	Pos += sizeof(struct grup_head);
	Pos += 16;
	//array(&grup->grups, 6);
	//array(&grup->records, 6);
	array(&grup->mixed, 12);
	// printf("G %.4s %u > ", (char *)&grup->head->type, grup->head->size);
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
	long size = grup->head->size - sizeof(struct grup_head) - 16;
	long start = Pos;
	while (Pos - start < size)
	{
	if (peek_type(esp) == ESP_GRUP_HEX)
	{
	void *element = read_grup(esp);
	//insert(&esp->grups, element);
	//insert(&grup->grups, element);
	insert(&grup->mixed, element);
	}
	else
	{
	void *element = read_record(esp);
	//insert(&esp->records, element);
	//insert(&grup->records, element);
	insert(&grup->mixed, element);
	}
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

static inline void copy(struct esp *esp, void *dest, size_t size, size_t count)
{
	memcpy(dest, Depos, size * count);
	Pos += size * count;
}