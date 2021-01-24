#include "putc.h"

#include "esp.h"

api void esp_print_form_id(Esp *esp, char *s, struct form_id *fi)
{
int w = snprintf(s, 200, "\
modIndex: %u\
\nobjectIndex: %u\
",
fi->modIndex,
fi->objectIndex
);
}

api void esp_print_grup(Esp *esp, char *s, Grup *grup)
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

api void esp_print_record(Esp *esp, char *s, Record *record)
{
int w = snprintf(s, 200, "\
id: %u\
\ntype: %.4s\
\nsize: %u\
\noffset: %u\
\nflags: %u\
\nformId: %u\
\nformId: %08X\
\ncompressed: %i\
\nfields: %i\
",
record->id,
(char *)&record->hed->type,
record->hed->size,
record->offset,
record->hed->flags,
record->hed->formId,
record->hed->formId,
(record->hed->flags & 0x00040000) != 0,
record->fields.size
);
}

char *specifics(Esp *esp, char *s, Subrecord *field)
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

api void esp_print_field(Esp *esp, char *s, Subrecord *field)
{
char x[300] = "Not Handled\0";
int w = snprintf(s, 400, "\
id: %u\
\ntype: %.4s\
\nsize: %u\
\noffset: %u\
\ndata: %p\
\nvalue: %s\
",
field->id,
(char *)&field->hed->type,
field->actualSize,
field->offset,
&field->data,
specifics(esp, x, field)
);
}

