// based on ortham/libespm

#include "c/c.h"

#include "esp.h"

#include <zlib.h>
#include <time.h>

struct record *read_record(struct esp *);
struct grup *read_grup(struct esp *);

#define Buf esp->buf
#define Pos esp->pos

int esp_skip_subrecords = 0;

struct esp *plugins[5] = { NULL, NULL, NULL, NULL, NULL };

unsigned int grups = 0;
unsigned int records = 0;
unsigned int subrecords = 0;
unsigned int decompressions = 0;

const char *esp_types[] = {"GMST", "KYWD", "LCRT", "AACT", "TXST", "GLOB", "CLAS", "FACT", "HDPT", "HAIR", "EYES", "RACE", "SOUN", "ASPC", "MGEF", "SCPT", "LTEX", "ENCH", "SPEL", "SCRL", "ACTI", "TACT", "ARMO", "BOOK", "CONT", "DOOR", "INGR", "LIGH", "MISC", "APPA", "STAT", "SCOL", "MSTT", "PWAT", "GRAS", "TREE", "CLDC", "FLOR", "FURN", "WEAP", "AMMO", "NPC_", "LVLN", "KEYM", "ALCH", "IDLM", "COBJ", "PROJ", "HAZD", "SLGM", "LVLI", "WTHR", "CLMT", "SPGD", "RFCT", "REGN", "NAVI", "CELL", "WRLD", "DIAL", "QUST", "IDLE", "PACK", "CSTY", "LSCR", "LVSP", "ANIO", "WATR", "EFSH", "EXPL", "DEBR", "IMGS", "IMAD", "FLST", "PERK", "BPTD", "ADDN", "AVIF", "CAMS", "CPTH", "VTYP", "MATT", "IPCT", "IPDS", "ARMA", "ECZN", "LCTN", "MESG", "RGDL", "DOBJ", "LGTM", "MUSC", "FSTP", "FSTS", "SMBN", "SMQN", "SMEN", "DLBR", "MUST", "DLVW", "WOOP", "SHOU", "EQUP", "RELA", "SCEN", "ASTP", "OTFT", "ARTO", "MATO", "MOVT", "HAZD", "SNDR", "DUAL", "SNCT", "SOPM", "COLL", "CLFM", "REVB"};

#define COUNT_OF(x) sizeof(x) / sizeof(0[x])

inline void array(struct esp_array *a, size_t initial) {
	a->capacity = initial;
	a->size = 0;
	a->elements = malloc(a->capacity * sizeof(void *));
}

inline void grow(struct esp_array *a) {
	if (a->size != a->capacity)
	return;
	a->capacity *= 2;
	a->elements = realloc(a->elements, a->capacity * sizeof(void *));
}

inline void insert(struct esp_array *a, void *element) {
	grow(a);
	a->elements[a->size++] = element;
}

float read_file(struct esp *esp)
{
	esp->file = fopen(esp->path, "rb");
	cassert_(esp->file, "esp can't open");
	clock_t before = clock();
	fseek(esp->file, 0, SEEK_END);
	esp->filesize = ftell(esp->file);
	printf("esp filesize %u\n", esp->filesize);
	rewind(esp->file);
	esp->buf = malloc(esp->filesize * sizeof(char));
	cassert_(esp->buf != NULL, "esp cant get memory");
	size_t read = fread(esp->buf, sizeof(char), esp->filesize, esp->file);
	cassert_(read, "esp buf bad");
	rewind(esp->file);
	clock_t after = clock();
	float difference = (float)(after - before) / CLOCKS_PER_SEC;
	printf("read esp %i mb took %.2fs\n", esp->filesize / 1024 / 1024, difference);
}

void make_top_grups(struct esp *);
void make_form_ids(struct esp *);

unsigned int hedr_num_records(struct esp *esp)
{
	if (esp_skip_subrecords)
	return 200;
	return *(unsigned int *)(esp->header->fields.fields[0]->data + 4);
}

api struct esp *esp_load(const char *path)
{
	grups = 0;
	records = 0;
	subrecords = 0;
	decompressions = 0;
	for (int i = 0; i < 5; i++)
	if (plugins[i] != NULL && 0 == strcmp(path, plugins[i]->path))
	return plugins[i];
	clock_t before, after;
	struct esp *esp = malloc(sizeof(struct esp));
	memset(esp, 0, sizeof(struct esp));
	strncpy(esp->path, path, 255);
	read_file(esp);
	before = clock();
	esp->header = read_record(esp);
	array(&esp->grups, 200);
	array(&esp->records, hedr_num_records(esp));
	while(Pos < esp->filesize)
	{
	struct grup *grup = read_grup(esp);
	insert(&esp->grups, grup);
	}
	make_top_grups(esp);
	make_form_ids(esp);
	fclose(esp->file);
	after = clock();
	float difference = (float)(after - before) / CLOCKS_PER_SEC;
	printf("parsing esp took %.2fs\n", difference);
	printf("esp has %i grups %i records %i subrecords %i decompressions\n", grups, records, subrecords, decompressions);
	return esp;
}

void uncompress_record(struct record *);
inline void read_record_subrecords(struct esp *, struct record *);

struct record *read_record(struct esp *esp)
{
	struct record *rec;
	rec = malloc(sizeof(struct record));
	rec->fi = NULL;
	// head
	rec->x = RECORD;
	rec->id = records++;
	rec->head = Buf + Pos;
	Pos += sizeof(struct record_head);
	Pos += 8;
	rec->actualSize = rec->head->size;
	rec->data = Buf + Pos;
	array(&rec->fields, 6);
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
	uncompress_record(rec);
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
	else
	insert(&rec->fields, sub);
	}
}

inline struct subrecord *read_subrecord(struct esp *esp, struct record *rec, unsigned int override)
{
	long *pos = &Pos;
	char *buf = Buf;
	if (rec->head->flags & 0x00040000)
	{
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
	sub->actualSize = override == 0 ? sub->head->size : override;
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
	//grup->lowest = grup->highest = 0;
	// head
	grup->x = GRUP;
	grup->id = grups++;
	grup->head = Buf + Pos;
	Pos += sizeof(struct grup_head);
	Pos += 16;
	grup->data = Buf + Pos;
	array(&grup->mixed, 12);
	// printf("G %.4s %u > ", (char *)&grup->head->type, grup->head->size);
	// records
	read_grup_records(esp, grup);
	// printf("\nend grup\n");
	return grup;
}

const unsigned int peek_type(struct esp *esp)
{
	return *(unsigned int *)(Buf + Pos);
}

inline void read_grup_records(struct esp *esp, struct grup *grup)
{
	long size = grup->head->size - sizeof(struct grup_head) - 16;
	long start = Pos;
	while (Pos - start < size)
	{
	if (peek_type(esp) == GRUP_HEX)
	insert(&grup->mixed, read_grup(esp));
	else
	insert(&grup->mixed, read_record(esp));
	}
}

void make_top_grups(struct esp *esp)
{
	const int max = COUNT_OF(esp_types);
	esp->tops = malloc(sizeof(const char *) * max);
	for (int i = 0; i < esp->grups.size; i++)
	{
	esp->tops[i] = "None";
	const struct grup *grup = esp->grups.elements[i];
	dive:
	if (grup->mixed.size == 0)
	continue;
	if(*(char *)grup->mixed.elements[0] != RECORD)
	{
	grup = grup->mixed.elements[0];
	goto dive;
	}
	struct record *first = grup->mixed.elements[0];
	for (int j = 0; j < max; j++)
	if (first->head->type == *(unsigned int *)esp_types[j])
	{
	esp->tops[i] = esp_types[j];
	break;
	}
	}
}

api struct grup *esp_get_top_grup(const struct esp *esp, const char type[5])
{
	for (int i = 0; i < esp->grups.size; i++)
	if (*(unsigned int *)type == *(unsigned int *)esp->tops[i])
	return esp->grups.elements[i];
	return NULL;
}

inline void build_form_id(struct esp *esp, struct record *record, struct form_id *fi)
{
	fi->esp = esp;
	record->fi = fi;
	fi->formId = record->head->formId;
	fi->record = record;
	//snprintf(form_id.hex, 10, "%08X", record->head->formId);
	//form_id.hex = 0;
	fi->modIndex = fi->formId >> 24;
	fi->objectIndex = fi->formId & ~(fi->modIndex << 24);

	//if (modIndex >= masters.size())
	//fi->plugin = parentPluginName;
	//else
	//fi->plugin = masters[fi->modIndex];
}

void make_form_ids(struct esp *esp)
{
	esp->formIds = malloc(sizeof(struct form_id) * esp->records.size);
	for (int i = 0; i < esp->records.size; i++)
	build_form_id(esp, esp->records.elements[i], &esp->formIds[i]);
}

api struct record *esp_brute_record_by_form_id(unsigned int formId)
{
	for (int p = 0; p < 5; p++)
	{
	struct esp *esp = plugins[p];
	if (esp == NULL)
	break;
	for (int i = 0; i < esp->records.size; i++)
	{
	struct record *rec = esp->records.elements[i];
	if (rec->fi->formId == formId)
	{
	//printf("found fi at rec %i\n", i);
	return rec;
	}
	}
	}
	return NULL;
}

api struct esp_array *esp_lazy_filter(const struct esp *esp, const char type[5])
{
	struct esp_array *filtered;
	filtered = malloc(sizeof(struct esp_array));
	array(filtered, 100);
	for (int i = 0; i < esp->records.size; i++)
	{
	struct record *record = esp->records.elements[i];
	if (record->head->type == *(unsigned int *)type)
	insert(filtered, record);
	}
	return filtered;
}

void uncompress_record(struct record *rec)
{
	char *src = rec->data;
	const unsigned int realSize = *(unsigned int *)src;
	const unsigned int size = rec->head->size - 4;
	src += 4;
	rec->buf = malloc(realSize * sizeof(char));
	int ret = uncompress(rec->buf, (uLongf*)&realSize, src, size);
	cassert_(ret == Z_OK, "esp zlib");
	rec->actualSize = realSize;
	decompressions++;
}

api void free_esp(struct esp **p)
{
	struct esp *esp = *p;
	*p = NULL;
	fclose(esp->file);
	free_esp_array(&esp->grups);
	free_esp_array(&esp->records);
	free(esp);
}

api void free_esp_array(struct esp_array *array)
{
	free(array->elements);
}