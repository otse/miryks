#include "c/c.h"

#include "esp.h"

#define Pos rec->pos

typedef int Parent;

static int read(struct esp *, void *, size_t, size_t);
static int seek(struct esp *, unsigned int);

struct record *read_record(struct esp *, Parent);
struct subrecord *read_subrecord(struct esp *, Parent);
struct grup *read_grup(struct esp *, Parent);
void read_grup_records(struct esp *, Parent);

void read_subrecord_data(struct esp *, struct subrecord *);

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
	int pos = ftell(esp->stream);
	while(pos < esp->filesize)
	{
	read_grup(esp, 0);
	pos = ftell(esp->stream);
	}
	return esp;
}

struct record *read_record(struct esp *esp, Parent parent) {
	struct record *rec = malloc(sizeof(struct record));
	memset(rec, 0, sizeof(struct record));
	//  header
	rec->x = RECORD;
	rec->id = Records++;
	rec->type[4] = '\0';
	read(esp, rec->type, 1, 4);
	read(esp, &rec->dataSize, 4, 1);
	read(esp, &rec->flags, 4, 1);
	read(esp, &rec->formId, 4, 1);
	int pos = ftell(esp->stream);
	seek(esp, pos += 8);
	// subrecords
	struct subrecord *prev = NULL, *first;
	int large = 0;
	while(rec->pos < rec->dataSize)
	{
	struct subrecord *next;
	next = read_subrecord(esp, rec->parent);
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

struct subrecord *read_subrecord(struct esp *esp, Parent parent) {
	struct subrecord *rec = malloc(sizeof(struct subrecord));
	memset(rec, 0, sizeof(struct subrecord));
	rec->x = SUBRECORD;
	rec->id = Subrecords++;
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

void peek_type(struct esp *esp, char type[5])
{
	type[4] = '\0';
	int pos = ftell(esp->stream);
	read(esp, type, 1, 4);
	seek(esp, pos);
}
struct grup *read_grup(struct esp *esp, Parent parent) {
	struct grup *grup = malloc(sizeof(struct grup));
	memset(grup, 0, sizeof(struct grup));
	// header
	grup->x = GRUP;
	grup->id = Grups++;
	grup->parent = parent;
	grup->type[4] = '\0';
	read(esp, grup->type, 1, 4);
	printf("grup %s", grup->type);
	read(esp, grup->size, 4, 1);
	int pos = ftell(esp->stream);
	seek(esp, pos + 16);
	// records
	read_grup_records(esp, grup);
}
void read_grup_records(struct esp *esp, struct grup *grup) {
	int size = grup->size - 4 - 4 - 16;
	int pos = ftell(esp->stream);
	while (pos < size)
	{
	char type[5];
	peek_type(esp, type);
	if (0 == strcmp("type", "GRUP"))
	read_grup(esp, grup->parent);
	else
	read_record(esp, grup->parent);
	seek(esp, pos + size);
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