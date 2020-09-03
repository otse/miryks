#include "c/c.h"

#include "esp.h"
#include "files.h"

api void esp_print_grup(struct esp *esp, char *s, struct grup *grup)
{
	int w = snprintf(s, 200, "\
id: %u\
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
id: %u\
\ntype: %.4s\
\ndataSize: %u\
\nflags: %u\
\nformId: %#08x\
\nsubrecords: %i\
",
record->id,
(char *)&record->head->type,
record->head->dataSize,
record->head->flags,
record->head->formId,
record->subrecords.used
);
}

char *print_edid(struct esp *esp, char *s, struct subrecord *subrecord)
{
	int w = snprintf(s, 255, "%s", subrecord->data+2);
	return s;
}
char *specifics(struct esp *esp, char *s, struct subrecord *subrecord)
{
	if (subrecord->head->type == ESP_EDID_HEX)
	print_edid(esp, s, subrecord);
	else
	*s = 0;
	return s;
}

api void esp_print_subrecord(struct esp *esp, char *s, struct subrecord *subrecord)
{
	char x[255];
	int w = snprintf(s, 400, "\
id: %u\
\ntype: %.4s\
\nsize: %u\
\ndata: %p\
\nvalue: %s\
",
subrecord->id,
(char *)&subrecord->head->type,
subrecord->actualSize,
&subrecord->data,
specifics(esp, x, subrecord)
);
}

