#include "c/c.h"

#include "esp.h"
#include "files.h"

api void esp_print_grup(struct esp *esp, char *s, struct grup *grup)
{
	int w = snprintf(s, 200, "\
grup: %i\
\ntype: %.4s\
\nsize: %u\
\ngrups: %u\
\nrecords: %u\
",
grup->id,
(char *)&grup->head->type,
grup->head->size,
grup->grups.used,
grup->records.used
);
}

api void esp_print_record(struct esp *esp, char *s, struct record *record)
{
	int w = snprintf(s, 200, "\
record: %i\
\ntype: %.4s\
\ndataSize: %u\
\nflags: %u\
\nformId: %#08x\
\nsubrecords: %i\
",
0,
(char *)&record->head->type,
record->head->dataSize,
record->head->flags,
record->head->formId,
record->subrecords.used
);
}

api void esp_print_subrecord(struct esp *esp, char *s, struct subrecord *subrecord)
{
	int w = snprintf(s, 400, "\
subrecord: %i\
\ntype: %.4s\
\nsize: %u\
\ndata: %p\
",
subrecord->id,
(char *)&subrecord->head->type,
subrecord->actualSize,
&subrecord->data);
}