#include "c/c.h"

#include "nif.h"

#define hedr nif->hdr
#define buf nif->buf
#define pos nif->pos
#define block nif->blocks[n]
#define from_buf() *(buf + pos);

#define one() pos += 1;
#define two() pos += 2;
#define four() pos += 4;
#define eight() pos += 8;

void nif_read_blocks(nif_t *nif)
{
	nif->blocks = malloc(sizeof(nif_block_t *) * hedr.num_blocks);
	for (int i = 0; i < hedr.num_blocks; i++)
	{
	nif->blocks[i] = malloc(sizeof(nif_block_t));
	nif_read_block(nif, i);
	}
    
}

void ni_node_and_friends(nif_t *, int);
void ni_skin_instance(nif_t *, int);
void ni_skin_data(nif_t *, int);

void nif_read_block(nif_t *nif, int n)
{
#define type(x) strcmp(block_type, x)
	const char *block_type = hedr.block_types[hedr.block_type_index[n]];
	if (0) 0;
	else if (type("NiNode") ||type("BSLeafAnimNode") ||type("BSFadeNode"))ni_node_and_friends(nif, n);
	else if (type("NiSkinInstance") ||type("BSDismemberSkinInstance"))ni_skin_instance(nif, n);
	else if (type("NiSkinData"))ni_skin_data(nif, n);
	else if (type("NiSkinPartition"))0;
	else if (type("BSTriShape") || type("BSDynamicTriShape"))0;
	else if (type("NiTriShape") || type("BSLODTriShape"))0;
	else if (type("NiAlphaProperty"))0;
	else if (type("NiTriShapeData"))2;
	else if (type("BSEffectShaderProperty"))2;
	else if (type("BSEffectShaderPropertyFloatController"))2;
	else if (type("NiFloatInterpolator"))2;
	else if (type("NiFloatData"))2;
	else if (type("BSLightingShaderProperty"))2;
	else if (type("BSShaderTextureSet"))2;
	else if (type("NiControllerSequence"))2;
	else if (type("NiTextKeyExtraData"))2;
	else if (type("NiStringExtraData"))2;
	else if (type("NiTransformInterpolator"))2;
	else if (type("NiTransformData"))2;
	else if (type("BSDecalPlacementVectorExtraData"))2;
}

void ni_node_and_friends(nif_t *nif, int n)
{

}

void ni_skin_instance(nif_t *nif, int n)
{

}

void ni_skin_data(nif_t *nif, int n)
{

}