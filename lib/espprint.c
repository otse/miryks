#include "c.h"

#include "esp.h"
#include "files.h"

api void esp_print_form_id(struct esp *esp, char *s, struct form_id *fi)
{
int w = snprintf(s, 200, "\
modIndex: %u\
\nobjectIndex: %u\
",
fi->modIndex,
fi->objectIndex
);
}

api void esp_print_grup(struct esp *esp, char *s, struct grup *grup)
{
int w = snprintf(s, 200, "\
id: %u\
\ntype: %.4s\
\nsize: %u\
\nelements: %u\
",
grup->id,
(char *)&grup->head->type,
grup->head->size,
grup->mixed.size
);
}

api void esp_print_record(struct esp *esp, char *s, struct record *record)
{
int w = snprintf(s, 200, "\
id: %u\
\ntype: %.4s\
\nsize: %u\
\nflags: %u\
\nformId: %u\
\nformId: %08X\
\ncompressed: %i\
\nfields: %i\
",
record->id,
(char *)&record->head->type,
record->head->size,
record->head->flags,
record->head->formId,
record->head->formId,
(record->head->flags & 0x00040000) != 0,
record->fields.size
);
}

char *specifics(struct esp *esp, char *s, struct field *field)
{
if (field->head->type == *(unsigned int *)"EDID")
snprintf(s, 300, "%s", field->data);
if (field->head->type == *(unsigned int *)"FULL")
snprintf(s, 300, "%s", field->data);
if (field->head->type == *(unsigned int *)"HEDR")
snprintf(s, 300, "\
\n  version: %.2f\
\n  numRecords: %u\
",
*(float *)(field->data),
*(unsigned int *)(field->data + 4));
	return s;
}

api void esp_print_field(struct esp *esp, char *s, struct field *field)
{
char x[300] = "Not Handled\0";
int w = snprintf(s, 400, "\
id: %u\
\ntype: %.4s\
\nsize: %u\
\ndata: %p\
\nvalue: %s\
",
field->id,
(char *)&field->head->type,
field->actualSize,
&field->data,
specifics(esp, x, field)
);
}

