#include "common.h"

#include "bsa.h"

#define INVERT_COMPRESSED 0x40000000

api void bsa_print_rc(Bsa *b, char *s, int r)
{
	Rc *rc = b->rc[r];
	int w = snprintf(s, 200, "\
resource #: %i\
\nfilename: %s\
\nfolder: %s\
\nbuf: %p\
\ncompressed: %i\
",
r,
rc->name,
b->ca[rc->i],
rc->buf,
(b->hdr.archive_flags & 0x4) != (b->file[rc->i][rc->j].size & 0x40000000)
);
}

api void bsa_print_fle_rcd(Bsa *b, char *s, int i, int j)
{
	struct bsa_file *rcd = &b->file[i][j];
	int w = snprintf(s, 200, "\
file #: %i\
\nhash: %llu\
\nsize: %lu\
\noffset: %lu\
",
b->r[i] + j,
rcd->hash,
rcd->size,
rcd->offset
);
}

api void bsa_print_fld_rcd(Bsa *b, char *s, int n)
{
	struct bsa_fld *rcd = &b->fld[n];
	int w = snprintf(s, 200, "\
folder #: %i\
\nhash: %llu\
\nnum files: %lu\
\noffset: %lu\
",
n,
rcd->hash,
rcd->num,
rcd->offset
);
}

api void bsa_print_hedr(Bsa *b, char *s)
{
#define hedr b->hdr
	int w = snprintf(s, 600, "\
id: %s\
\nver: %i\
\nfolders: %lu\
\nfiles: %lu\
\nstrings: %lu, %lu\
\nflags:\
\n 0x1   %i\
\n 0x2   %i\
\n 0x4   %i\
\n 0x8   %i\
\n 0x10  %i\
\n 0x20  %i\
\n 0x40  %i\
\n 0x80  %i\
\n 0x100 %i\
\n 0x200 %i\
\n 0x400 %i\
\nfile flags:\
\n meshes   %i\
\n textures %i\
\n menus    %i\
\n sounds   %i\
\n voices   %i\
\n shaders  %i\
\n trees    %i\
\n fonts    %i\
\n misc     %i\
",
hedr.id,
hedr.ver,
hedr.folders,
hedr.files,
hedr.foldersl,
hedr.filesl,
hedr.archive_flags & 0x1,
hedr.archive_flags & 0x2,
hedr.archive_flags & 0x4,
hedr.archive_flags & 0x8,
hedr.archive_flags & 0x10,
hedr.archive_flags & 0x20,
hedr.archive_flags & 0x40,
hedr.archive_flags & 0x80,
hedr.archive_flags & 0x100,
hedr.archive_flags & 0x200,
hedr.archive_flags & 0x400,
hedr.file_flags & BSA_MESHES,
hedr.file_flags & BSA_TEXTURES,
hedr.file_flags & BSA_MENUS,
hedr.file_flags & BSA_SOUNDS,
hedr.file_flags & BSA_VOICES,
hedr.file_flags & BSA_SHADERS,
hedr.file_flags & BSA_TREES,
hedr.file_flags & BSA_FONTS,
hedr.file_flags & BSA_MISC
);
}