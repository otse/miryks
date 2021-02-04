#include "putc.h"

#include "esp.h"

#include <zlib.h>
#include <sys/stat.h>

// only for oldrim!

// wrote for gloom/dark
// 100% based on https://github.com/Ortham/libespm

// Subrecord *read_subrecord ?
Record *read_record(Esp *);
Grup *read_grup(Esp *);

#define Buf esp->buf
#define Pos esp->pos

#define Count esp->count

int esp_skip_fields = 0;

static Esp *plugins[5] = { NULL, NULL, NULL, NULL, NULL };

// Todo, wooo
const char *esp_types[] = {"GMST", "KYWD", "LCRT", "AACT", "TXST", "GLOB", "CLAS", "FACT", "HDPT", "HAIR", "EYES", "RACE", "SOUN", "ASPC", "MGEF", "SCPT", "LTEX", "ENCH", "SPEL", "SCRL", "ACTI", "TACT", "ARMO", "BOOK", "CONT", "DOOR", "INGR", "LIGH", "MISC", "APPA", "STAT", "SCOL", "MSTT", "PWAT", "GRAS", "TREE", "CLDC", "FLOR", "FURN", "WEAP", "AMMO", "NPC_", "LVLN", "KEYM", "ALCH", "IDLM", "COBJ", "PROJ", "HAZD", "SLGM", "LVLI", "WTHR", "CLMT", "SPGD", "RFCT", "REGN", "NAVI", "CELL", "WRLD", "DIAL", "QUST", "IDLE", "PACK", "CSTY", "LSCR", "LVSP", "ANIO", "WATR", "EFSH", "EXPL", "DEBR", "IMGS", "IMAD", "FLST", "PERK", "BPTD", "ADDN", "AVIF", "CAMS", "CPTH", "VTYP", "MATT", "IPCT", "IPDS", "ARMA", "ECZN", "LCTN", "MESG", "RGDL", "DOBJ", "LGTM", "MUSC", "FSTP", "FSTS", "SMBN", "SMQN", "SMEN", "DLBR", "MUST", "DLVW", "WOOP", "SHOU", "EQUP", "RELA", "SCEN", "ASTP", "OTFT", "ARTO", "MATO", "MOVT", "HAZD", "SNDR", "DUAL", "SNCT", "SOPM", "COLL", "CLFM", "REVB"};

#define COUNT_OF(x) sizeof(x) / sizeof(0[x])

inline void array(EspCArray *, size_t);
inline void grow(EspCArray *);
inline void insert(EspCArray *, void *);

void make_top_grups(Esp *);
void make_form_ids(Esp *);

unsigned int hedr_num_records(Esp *esp)
{
	if (esp_skip_fields)
	return 200;
	return *(unsigned int *)(esp->header->fields.subrecords[0]->data + 4);
}

Esp *plugin_slate()
{
	Esp *plugin = malloc(sizeof(Esp));
	memset(plugin, 0, sizeof(Esp));
	return plugin;
}

api int plugin_load(Esp *esp)
{
	cassert(esp->buf,      "need buf");
	cassert(esp->filesize, "need filesize");
	cassert(esp->name,     "need name");
	esp->header = read_record(esp);
	array(&esp->grups, 200);
	array(&esp->records, hedr_num_records(esp));
	while(Pos < esp->filesize)
	{
	Grup *grup = read_grup(esp);
	insert(&esp->grups, grup);
	}
	make_top_grups(esp);
	make_form_ids(esp);
	return 1;
}

void uncompress_record(Esp *, Record *);
inline void read_record_fields(Esp *, Record *);

Record *read_record(Esp *esp)
{
	// Todo, clean big unclear assignments lik these thruout the program
	Record *rec;
	rec = malloc(sizeof(Record));
	rec->fi = NULL;
	// head
	rec->x = RECORD;
	rec->indices = 0;
	rec->id = Count.records++;
	rec->offset = Pos;
	rec->hed = Buf + Pos;
	Pos += sizeof(struct record_header);
	Pos += 8;
	rec->actualSize = rec->hed->size;
	rec->data = Buf + Pos;
	array(&rec->fields, 6);
	insert(&esp->records, rec);
	// printf("R %.4s %u > ", (char *)&rec->hed->type, rec->hed->dataSize);
	// fields
	if (esp_skip_fields)
	Pos += rec->hed->size;
	else
	{
	if (rec->hed->flags & 0x00040000)
	{
	rec->buf = 0; rec->pos = 0;
	uncompress_record(esp, rec);
	read_record_fields(esp, rec);
	Pos += rec->hed->size;
	}
	else
	read_record_fields(esp, rec);
	}
	return rec;
}

inline Subrecord *read_field(Esp *, Record *, unsigned int);

inline void read_record_fields(Esp *esp, Record *rec)
{
	long *pos = &Pos;
	if (rec->hed->flags & 0x00040000)
	pos = &rec->pos;
	long start = *pos;
	unsigned int large = 0;
	while(*pos - start < rec->actualSize)
	{
	Subrecord *sub;
	sub = read_field(esp, rec, large);
	large = 0;
	if (sub->hed->type == *(unsigned int *)"XXXX")
	large = *(unsigned int *)sub->data;
	else
	insert(&rec->fields, sub);
	}
}

inline Subrecord *read_field(Esp *esp, Record *rec, unsigned int override)
{
	long *pos = &Pos;
	char *buf = Buf;
	if (rec->hed->flags & 0x00040000)
	{
	pos = &rec->pos;
	buf = rec->buf;
	}
	Subrecord *sub;
	sub = malloc(sizeof(Subrecord));
	// hed
	sub->x = SUBRECORD;
	sub->index = rec->indices++;
	sub->id = Count.fields++;
	sub->offset = Pos;
	sub->hed = buf + *pos;
	*pos += sizeof(struct field_header);
	sub->actualSize = override == 0 ? sub->hed->size : override;
	// data
	sub->data = buf + *pos;
	*pos += sub->actualSize;
	// printf("S %.4s %u > ", (char *)&sub->hed->type, sub->hed->size);
	return sub;
}

inline void read_grup_records(Esp *, Grup *);


Grup *read_grup(Esp *esp)
{
	Grup *grup = malloc(sizeof(Grup));
	//grup->lowest = grup->highest = 0;
	// hed
	grup->x = GRUP;
	grup->id = Count.grups++;
	grup->hed = Buf + Pos;
	Pos += sizeof(struct grup_header);
	Pos += 16;
	grup->data = Buf + Pos;
	array(&grup->mixed, 12);
	// printf("G %.4s %u > ", (char *)&grup->hed->type, grup->hed->size);
	// records
	read_grup_records(esp, grup);
	// printf("\nend grup\n");
	return grup;
}

const unsigned int peek_type(Esp *esp)
{
	return espwrd (Buf + Pos);
}

inline void read_grup_records(Esp *esp, Grup *grup)
{
	long size = grup->hed->size - sizeof(struct grup_header) - 16;
	long start = Pos;
	while (Pos - start < size)
	{
	if (peek_type(esp) == espwrd "GRUP")
	insert(&grup->mixed, read_grup(esp));
	else
	insert(&grup->mixed, read_record(esp));
	}
}

void make_top_grups(Esp *esp)
{
	const int max = COUNT_OF(esp_types);
	esp->tops = malloc(sizeof(const char *) * max);
	for (unsigned int i = 0; i < esp->grups.size; i++)
	{
	esp->tops[i] = "None";
	const Grup *grup = esp->grups.elements[i];
	dive:
	if (grup->mixed.size == 0)
	continue;
	if(*(char *)grup->mixed.elements[0] != RECORD)
	{
	grup = grup->mixed.elements[0];
	goto dive;
	}
	Record *first = grup->mixed.elements[0];
	for (int j = 0; j < max; j++)
	if (first->hed->type == *(unsigned int *)esp_types[j])
	{
	esp->tops[i] = esp_types[j];
	break;
	}
	}
}

api Grup *esp_get_top_grup(const Esp *esp, const char type[5])
{
	for (unsigned int i = 0; i < esp->grups.size; i++)
	if (*(unsigned int *)type == *(unsigned int *)esp->tops[i])
	return esp->grups.elements[i];
	return NULL;
}

inline void build_form_id(Esp *esp, Record *record, struct form_id *fi)
{
	fi->esp = esp;
	record->fi = fi;
	fi->formId = record->hed->formId;
	fi->record = record;
	//snprintf(form_id.hex, 10, "%08X", record->hed->formId);
	//form_id.hex = 0;
	fi->modIndex = fi->formId >> 24;
	fi->objectIndex = fi->formId & ~(fi->modIndex << 24);
	//if (modIndex >= masters.size())
	//fi->plugin = parentPluginName;
	//else
	//fi->plugin = masters[fi->modIndex];
}

void make_form_ids(Esp *esp)
{
	// Unused
	EspCArray *array = &esp->records;
	esp->formIds = malloc(sizeof(struct form_id) * array->size);
	for (unsigned int i = 0; i < array->size; i++)
	build_form_id(esp, array->elements[i], &esp->formIds[i]);
}

api Record *esp_get_form_id(unsigned int formId)
{
	for (int i = 5; i --> 0; )
	{
	Esp *esp = plugins[i];
	if (esp == NULL)
	continue;
	for (unsigned int j = 0; j < esp->records.size; j++)
	{
	Record *rec = esp->records.elements[j];
	if (rec->fi->formId == formId)
	return rec;
	}
	}
	return NULL;
}

/*
api void esp_array_loop(EspCArray *array, void (*func)(Subrecord *field, void *data), void *data)
{
	for (int i = 0; i < array->size; i++)
	{
	func(array->elements[i], data);
	};
}
*/

// Util code I did for imgui, kinda useless once I learned about top grups
api EspCArray *esp_filter_objects(const Esp *esp, const char type[5])
{
	EspCArray *filtered;
	filtered = malloc(sizeof(EspCArray));
	array(filtered, 100);
	for (unsigned int i = 0; i < esp->records.size; i++)
	{
	Record *record = esp->records.elements[i];
	if (record->hed->type == *(unsigned int *)type)
	insert(filtered, record);
	}
	return filtered;
}

void uncompress_record(Esp *esp, Record *rec)
{
	// Workhorse code that is similar to how Bsa does it
	char *src = rec->data;
	const unsigned int realSize = *(unsigned int *)src;
	const unsigned int size = rec->hed->size - 4;
	src += 4;
	rec->buf = malloc(realSize * sizeof(char));
	int ret = uncompress(rec->buf, (uLongf*)&realSize, src, size);
	cassert(ret == Z_OK, "esp zlib");
	rec->actualSize = realSize;
	Count.uncompress++;
}

api Esp **get_plugins()
{
	return plugins;
}

api Esp *has_plugin(const char *name)
{
	for (int i = 5; i-- > 0;)
	if (plugins[i] != NULL && 0 == strcmp(name, plugins[i]->name))
	return plugins[i];
	return NULL;
}

api void free_plugin(Esp **p)
{
	Esp *esp = *p;
	*p = NULL;
	if (esp == NULL)
	return;
	for (int i = 5; i --> 0; )
	if (esp == plugins[i])
	plugins[i] = NULL;
	for (unsigned int i = 0; i < esp->records.size; i++)
	{
	Record *record = esp->records.elements[i];
	if (record->hed->flags & 0x00040000)
	free(record->buf);
	free_esp_array(&record->fields);
	}
	free_esp_array(&esp->grups);
	free_esp_array(&esp->records);
	free(esp->buf);
	free(esp);
}

// Horrible growable c array stolen from GitHub

api void free_esp_array(EspCArray *array)
{
	free(array->elements);
}

inline void array(EspCArray *a, size_t initial) {
	a->capacity = initial;
	a->size = 0;
	a->elements = malloc(a->capacity * sizeof(void *));
}

inline void grow(EspCArray *a) {
	if (a->size != a->capacity)
	return;
	a->capacity *= 2;
	a->elements = realloc(a->elements, a->capacity * sizeof(void *));
}

inline void insert(EspCArray *a, void *element) {
	grow(a);
	a->elements[a->size++] = element;
}