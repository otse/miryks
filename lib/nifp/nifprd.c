#include "putc.h"

#include "nifp.h"

#define Hedr   nif->hdr
#define Buf    nif->buf
#define Pos    nif->pos
#define Blocks nif->blocks
#define Depos  Buf + Pos

void visit(struct nifprd *, int, int);
void visit_other(struct nifprd *, int, int);
void visit_block(struct nifprd *, void *);

api struct nifprd *malloc_nifprd() {
	struct nifprd *rd = malloc(sizeof(struct nifprd));
	memset(rd, 0, sizeof(struct nifprd));
	rd->other = visit_other;
	rd->ni_node = rd->ni_tri_shape = rd->ni_tri_shape_data = rd->bs_lighting_shader_property = rd->bs_shader_texture_set = visit_block;
	return rd;
}

api void free_nifrd(struct nifprd **p) {
	struct nifprd *rd = *p;
	free(rd->skips);
	free(rd);
	*p = NULL;
}

api void nifp_rd(struct nifp *nif, struct nifprd *rd) {
	// printf("nif rd\n");
	rd->skips = malloc(sizeof(int) * Hedr->num_blocks);
	memset(rd->skips, 0, sizeof(int) * Hedr->num_blocks);
	rd->nif = nif;
	for (int n = 0; n < Hedr->num_blocks; n++)
	{
	visit(rd, -1, n);
	}
}

static void visit(struct nifprd *rd, int p, int c)
{
#define skip rd->skips[c] = 1;
#define needs_parent if (-1 == p) return;
	struct nifp *nif = rd->nif;
	if (-1 == c)
	return;
	if (rd->skips[c])
	return;
	rd->parent = p; rd->current = c;
	// skip
	const char *block_type = Hedr->block_types[Hedr->block_type_index[c]];
	if (0) ;
	else if ( ni_is_any(NI_NODE, BS_LEAF_ANIM_NODE, BS_FADE_NODE) )
	{
	skip;
	struct ni_node_pointer *block = Blocks[c];
	rd->ni_node(rd, block);
	for (int i = 0; i < block->A->num_children; i++)
	{
	int b = block->children[i];
	visit(rd, c, b);
	}
	}
	else if ( ni_is_any(NI_TRI_SHAPE, BS_LOD_TRI_SHAPE, NULL) )
	{
	skip;
	struct ni_tri_shape_pointer *block = Blocks[c];
	rd->ni_tri_shape(rd, block);
	visit(rd, c, block->A->data);
	visit(rd, c, block->B->shader_property);
	visit(rd, c, block->B->alpha_property);
	}
	else if ( ni_is_type(NI_TRI_SHAPE_DATA) )
	{
	needs_parent;
	rd->ni_tri_shape_data(rd, Blocks[c]);
	}
	else if ( ni_is_type(BS_LIGHTING_SHADER_PROPERTY) )
	{
	needs_parent;
	struct bs_lighting_shader_property_pointer *block = Blocks[c];
	rd->bs_lighting_shader_property(rd, block);
	visit(rd, c, block->texture_set);
	}
	else if ( ni_is_type(BS_SHADER_TEXTURE_SET) )
	{
	needs_parent;
	rd->bs_shader_texture_set(rd, Blocks[c]);
	}
	else if ( ni_is_type(NI_ALPHA_PROPERTY) )
	{
	needs_parent;
	rd->ni_alpha_property(rd, Blocks[c]);
	}
}

static void visit_other(struct nifprd *rd, int p, int c) {}
static void visit_block(struct nifprd *rd, void *block) {}