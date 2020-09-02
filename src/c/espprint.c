#include "c/c.h"

#include "esp.h"
#include "files.h"

api void esp_print_record(struct esp *esp, char *s, struct record *record)
{
	int w = snprintf(s, 200, "\
record: %i\
\ntype: %s\
\ndataSize: %u\
\nflags: %u\
\nformId: %#08x\
\ncount: %i\
",
0,
record->head->type,
record->head->dataSize,
record->head->flags,
record->head->formId,
-1);
}

api void esp_print_subrecord(struct esp *esp, char *s, struct subrecord *subrecord)
{
	int w = snprintf(s, 400, "\
record: %i\
\ntype: %s\
\nsize: %u\
\ndata: %p\
",
0,
subrecord->head->type,
subrecord->size,
&subrecord->data);
}