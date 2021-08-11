#include "common.h"

#include "esp.h"

api void esp_print_form_id(espp esp, char *s, struct form_id *fi)
{
int w = snprintf(s, 200, "\
modIndex: %u\
\nobjectIndex: %u\
",
fi->modIndex,
fi->objectIndex
);
}

#define LOW(x) ((x) & 0xff)
#define HIGH(x) ((x) >> 8)

api void esp_print_grup(espp esp, char *s, grupp grup)
{
int w = snprintf(s, 200, "\
id: %u\
\ntype: %.4s\
\nsize: %u\
\nlabel: %.4s\
\ngroup_type: %i\
\ntime_stamp: %hu\
\nversion_control_info: %02x %02x\
\nelements: %u\
",
grup->id,
(char *)&grup->hed->sgn,
grup->hed->size,
(char *)&grup->hed->label,
grup->hed->group_type,
grup->hed->time_stamp,
LOW(grup->hed->version_control_info),
HIGH(grup->hed->version_control_info),
grup->mixed->size
);
}

api void esp_print_record(espp esp, char *s, record *record)
{
int w = snprintf(s, 200, "\
id: %u\
\ntype: %.4s\
\nsize: %u\
\noffset: %u\
\nflags: %u\
\nformId: %u\
\nformId: %08X\
\nflags\
\n compressed: %i\
\ntime_stamp: %hu\
\nversion_control_info: %02x %02x\
\nform_version: %hu\
\nfields: %i\
",
record->id,
(char *)&record->hed->sgn,
record->hed->size,
record->offset,
record->hed->flags,
record->hed->formId,
record->hed->formId,
(record->hed->flags & 0x00040000) != 0,
record->hed->time_stamp,
LOW(record->hed->version_control_info),
HIGH(record->hed->version_control_info),
record->hed->form_version,
record->subrecords->size
);
}

char *specifics(espp esp, char *s, subrecord *field)
{
if (field->hed->sgn == *(unsigned int *)"EDID")
snprintf(s, 300, "%s", field->data);
if (field->hed->sgn == *(unsigned int *)"FULL")
snprintf(s, 300, "%s", field->data);
if (field->hed->sgn == *(unsigned int *)"MAST")
snprintf(s, 300, "%s", field->data);
if (field->hed->sgn == *(unsigned int *)"HEDR")
snprintf(s, 300, "\
\n  version: %.2f\
\n  numRecords: %u\
",
*(float *)(field->data),
*(unsigned int *)(field->data + 4));
	return s;
}

api void esp_print_field(espp esp, char *s, subrecord *field)
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
(char *)&field->hed->sgn,
field->actualSize,
field->offset,
&field->data,
specifics(esp, x, field)
);
}

