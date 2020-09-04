// based on ortham/libespm

#include "c/c.h"

#include "esp.h"

#include <zlib.h>
#include <time.h>

struct record *read_record(struct esp *);
struct grup *read_grup(struct esp *);

#define Buf esp->buf
#define Pos esp->pos
#define Depos Buf + Pos

int report = 0;
int esp_skip_subrecords = 0;

unsigned int grups = 0;
unsigned int records = 0;
unsigned int subrecords = 0;

inline void array(struct esp_array *a, size_t initial, size_t element) {
	a->size = initial;
	a->used = 0;
	a->element = element;
	a->array = malloc(a->size * a->element);
}

inline void grow(struct esp_array *a) {
	if (a->used != a->size)
	return;
	a->size *= 2;
	a->array = realloc(a->array, a->size * a->element);
}

inline void insert(struct esp_array *a, void *element) {
	grow(a);
	a->pointers[a->used++] = element;
}

// todo cleanup pls
float read_entire_file(struct esp *esp)
{
	clock_t before, after;
	before = clock();
	fseek(esp->file, 0, SEEK_END);
	esp->filesize = ftell(esp->file);
	printf("esp filesize %u\n", esp->filesize);
	rewind(esp->file);
	esp->buf = malloc(sizeof(char) * esp->filesize);
	cassert_(esp->buf != NULL, "esp cant get memory");
	size_t read = fread(esp->buf, sizeof(char), esp->filesize, esp->file);
	cassert_(read, "esp buf bad");
	rewind(esp->file);
	after = clock();
	float difference = (float)(after - before) / CLOCKS_PER_SEC;
	printf("read entire esp %i mb took %.2fs\n", esp->filesize / 1024 / 1024, difference);
	return difference;
}

// todo cleanup pls

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
	array(&esp->formIds, 2000, sizeof(struct form_id));
	array(&esp->grups, 200, sizeof(void *));
	array(&esp->records, 200, sizeof(void *));
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

inline void form_id(struct form_id *form_id, struct record *record)
{
	form_id->formId = record->head->formId;
	#if FORMID_HEX
	snprintf(form_id->hex, 10, "%08X", form_id->formId);
	#else
	form_id.hex = formId;
	#endif
}

void uncompress_record(struct esp *, struct record *);
inline void read_record_subrecords(struct esp *, struct record *);

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
	rec->actualSize = rec->head->size;
	rec->data = Depos;
	array(&rec->fields, 6, sizeof(void *));
	insert(&esp->records, rec);
	// printf("R %.4s %u > ", (char *)&rec->head->type, rec->head->dataSize);
	// subrecords
	if (esp_skip_subrecords)
	Pos += rec->head->size;
	else
	{
	if (rec->head->flags & 0x00040000)
	{
	rec->buf = rec->pos = 0;
	uncompress_record(esp, rec);
	read_record_subrecords(esp, rec);
	Pos += rec->head->size;
	}
	else
	read_record_subrecords(esp, rec);
	}
	return rec;
}

inline struct subrecord *read_subrecord(struct esp *, struct record *, unsigned int);

inline void read_record_subrecords(struct esp *esp, struct record *rec)
{
	long *pos = &Pos;
	if (rec->head->flags & 0x00040000)
	pos = &rec->pos;
	long start = *pos;
	unsigned int large = 0;
	while(*pos - start < rec->actualSize)
	{
	struct subrecord *sub;
	sub = read_subrecord(esp, rec, large);
	large = 0;
	if (sub->head->type == XXXX_HEX)
	large = *(unsigned int *)sub->data;
	insert(&rec->fields, sub);
	}
}

inline struct subrecord *read_subrecord(struct esp *esp, struct record *rec, unsigned int override)
{
	long *pos = &Pos;
	char *buf = Buf;
	if (rec->head->flags & 0x00040000)
	{
	// printf("boy read comp field\n");
	pos = &rec->pos;
	buf = rec->buf;
	}
	struct subrecord *sub;
	sub = malloc(sizeof(struct subrecord));
	// head
	sub->x = SUBRECORD;
	sub->id = subrecords++;
	sub->head = buf + *pos;
	*pos += sizeof(struct subrecord_head);
	if (override)
	sub->actualSize = override;
	else
	sub->actualSize = sub->head->size;
	// data
	sub->data = buf + *pos;
	*pos += sub->actualSize;
	// printf("S %.4s %u > ", (char *)&sub->head->type, sub->head->size);
	return sub;
}

inline void read_grup_records(struct esp *, struct grup *);

struct grup *read_grup(struct esp *esp)
{
	struct grup *grup = malloc(sizeof(struct grup));
	// head
	grup->x = GRUP;
	grup->id = grups++;
	grup->head = Depos;
	Pos += sizeof(struct grup_head);
	Pos += 16;
	grup->data = Depos;
	array(&grup->mixed, 12, sizeof(void **));
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

inline void read_grup_records(struct esp *esp, struct grup *grup)
{
	long size = grup->head->size - sizeof(struct grup_head) - 16;
	long start = Pos;
	while (Pos - start < size)
	{
	if (peek_type(esp) == GRUP_HEX)
	{
	void *element = read_grup(esp);
	insert(&grup->mixed, element);
	}
	else
	{
	void *element = read_record(esp);
	insert(&grup->mixed, element);
	}
	}
}

void uncompress_record(struct esp *esp, struct record *rec)
{
	char *src = rec->data;
	unsigned int realSize = *(unsigned int *)src;
	unsigned int size = rec->head->size - 4;
	src += 4;
	rec->buf = malloc(realSize * sizeof(char));
	int ret = uncompress(rec->buf, (uLongf*)&realSize, src, size);
	cassert_(ret == Z_OK, "esp zlib");
	rec->actualSize = realSize;
}

api struct esp_array *esp_filter_records(struct esp *esp, char s[4])
{
	struct esp_array *filtered = malloc(sizeof(struct esp_array));
	unsigned int type = *(unsigned int *)s;
	array(filtered, 100, sizeof(void *));
	for (int i = 0; i < esp->records.used; i++)
	{
	struct record *record = esp->records.pointers[i];
	if (record->head->type == type)
	{
	insert(filtered, record);
	}
	}
	printf("filtered %u recs\n", filtered->used);
	return filtered;
}

api void free_esp(struct esp **p)
{
	struct esp *esp = *p;
	*p = NULL;
	fclose(esp->file);
	free(esp);
}

api void free_esp_array(struct esp_array **p)
{
	struct esp_array *array = *p;
	*p = NULL;
	free(array->array);
	free(array);
}