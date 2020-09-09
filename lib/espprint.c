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
(char *)&grup->hed->type,
grup->hed->size,
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
(char *)&record->hed->type,
record->hed->size,
record->hed->flags,
record->hed->formId,
record->hed->formId,
(record->hed->flags & 0x00040000) != 0,
record->fields.size
);
}

char *specifics(struct esp *esp, char *s, struct subrecord *field)
{
if (field->hed->type == *(unsigned int *)"EDID")
snprintf(s, 300, "%s", field->data);
if (field->hed->type == *(unsigned int *)"FULL")
snprintf(s, 300, "%s", field->data);
if (field->hed->type == *(unsigned int *)"HEDR")
snprintf(s, 300, "\
\n  version: %.2f\
\n  numRecords: %u\
",
*(float *)(field->data),
*(unsigned int *)(field->data + 4));
	return s;
}

api void esp_print_field(struct esp *esp, char *s, struct subrecord *field)
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
(char *)&field->hed->type,
field->actualSize,
&field->data,
specifics(esp, x, field)
);
}

