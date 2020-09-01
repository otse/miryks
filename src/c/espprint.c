#include "c/c.h"

#include "esp.h"
#include "files.h"

api void esp_print_record(struct esp *esp, char *s, struct record *record)
{
	int w = snprintf(s, 200, "\
record: %i\
\ntype: %s\
\nsubrecordsSize: %u\
\nflags: %u\
\ncount: %u\
",
0,
record->type,
record->subrecordsSize,
record->flags,
record->count);
}

api void esp_print_subrecord(struct esp *esp, char *s, struct subrecord *subrecord)
{
	int w = snprintf(s, 400, "\
record: %i\
\ntype: %s\
\nlength: %u\
\nbuf: %s\
",
0,
subrecord->type,
subrecord->length,
subrecord->buf);
}