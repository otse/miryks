#include "c/c.h"

#include "esp.h"

#define Pos rec->pos

static int read(struct esp *, void *, size_t, size_t);
static int seek(struct esp *, unsigned int);

struct record *read_record_header(struct esp *, int);
struct subrecord *read_subrecord(struct esp *, int);
struct group *read_group(struct esp *, int);
void read_group_records(struct esp *, int);

void read_subrecord_data(struct esp *, struct subrecord *);

api struct esp *esp_load(const char *path) {
	printf("esp load\n");
	struct esp *esp = malloc(sizeof(struct esp));
	memset(esp, 0, sizeof(struct esp));
	esp->path = malloc(sizeof(char) * strlen(path) + 1);
	strcpy(esp->path, path);
	esp->stream = fopen(path, "rb");
	cassert_(
		esp->stream, "esp can't open");
	esp->header = read_record_header(esp, 0);
	return esp;
}

struct record *read_record_header(struct esp *esp, int i) {
	struct record *rec = malloc(sizeof(struct record));
	memset(rec, 0, sizeof(struct record));
	rec->type[4] = '\0';
	read(esp, rec->type, 1, 4);
	read(esp, &rec->subrecordsSize, 4, 1);
	read(esp, &rec->flags, 4, 1);
	read(esp, &rec->formId, 4, 1);
	int pos = ftell(esp->stream);
	seek(esp, pos + 8);
	struct subrecord *prev = NULL, *first;
	int large = 0;
	while(rec->pos < rec->subrecordsSize)
	{
		struct subrecord *next;
		next = read_subrecord(esp, 0);
		rec->count += 1;
		rec->pos += 4 + 2 + next->length;
		if (prev == NULL)
		first = next;
		else
		prev->next = next;
		prev = next;
		if (0 == strcmp(next->type, "XXXX"))
		{
			printf("LARGE");
		}
	}
	rec->subrecords = malloc(sizeof(struct subrecord *) * rec->count);
	struct subrecord *loop = first;
	for (int i = 0; i < rec->count; i++)
	{
	rec->subrecords[i] = loop;
	loop = loop->next;
	}
	return rec;
}

struct subrecord *read_subrecord(struct esp *esp, int i) {
	struct subrecord *rec = malloc(sizeof(struct subrecord));
	memset(rec, 0, sizeof(struct subrecord));
	rec->type[4] = '\0';
	read(esp, rec->type, 1, 4);
	read(esp, &rec->length, 2, 1);
	#if 1
	read_subrecord_data(esp, rec);
	#else
	int pos = ftell(esp->stream);
	rec->start = pos;
	seek(esp, pos + rec->length);
	#endif
	return rec;
}

void read_subrecord_data(struct esp *esp, struct subrecord *rec) {
	//seek(esp, rec->start);
	rec->buf = malloc(sizeof(char) * rec->length);
	read(esp, rec->buf, rec->length, 1);
}

struct group *read_group(struct esp *esp, int i) {
	struct group *group = malloc(sizeof(struct group));
	memset(group, 0, sizeof(struct group));
	group->type[4] = '\0';
	read(esp, group->type, 1, 4);
	read(esp, group->size, 4, 1);
	int pos = ftell(esp->stream);
	seek(esp, pos + 16);
}

void read_group_records(struct esp *esp, int i) {

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
	return fseek((FILE *)esp->stream, offset, SEEK_SET);
}