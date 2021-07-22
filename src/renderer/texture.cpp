#include <dark/dark.h>

#include <renderer/texture.h>

extern "C"
{
#include "common.h"
#include "bsa.h"
#include "dds.h"
}

#include <algorithm>

static std::map<const std::string, Texture *> textures;

inline std::string lowercase(const char *c)
{
	std::string s = c;
	std::transform(s.begin(), s.end(), s.begin(), ::tolower);
	return s;
}

Texture *GetProduceTexture(const char *path)
{
	// printf("getproducetexture %s\n", path);
	std::string lower = lowercase(path);
	if (textures.count(lower) == 0)
		textures.emplace(lower, new Texture(lower));
	return textures[lower];
}

bool test = false;

Texture::Texture()
{
}

Texture::Texture(const std::string &path) : path(path)
{
	//printf("new texture %s\n", path.c_str());
	Rc *rc = bsa_find_more(path.c_str(), 0x2);
	if (rc == NULL) {
		printf("cant find texture: %s\n", path.c_str());
		return;
	}
	bsa_read(rc);
	buf = rc->buf;
	size = rc->size;
	if (!test)
	{
		cfout2("test.dds", rc->buf, rc->size);
		test = true;
	}
	load();
}
Texture::~Texture() {}

void Texture::load()
{
	DDSFile *dds = ddsloader_load_buf(buf, size);

	int offset = 0;
	int count = dds->dwMipMapCount;
	int w = dds->dwWidth;
	int h = dds->dwHeight;

	// for now this works
	if (count > 10)
	{
		count -= 4;
	}

	glGenTextures(1, &tid);

	glBindTexture(GL_TEXTURE_2D, tid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, count - 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // don't forget to enable mipmaping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	GLfloat value, max_anisotropy = 8.0f; /* don't exceed this value...*/
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &value);

	value = (value > max_anisotropy) ? max_anisotropy : value;
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, value);

	char *fourcc = (char *)&(dds->ddspf.dwFourCC);

	//printf("dds four cc ", string_t(fourcc));

	unsigned int format;

	if (strncmp(fourcc, "DXT1", 4) == 0)
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;

	else if (strncmp(fourcc, "DXT3", 4) == 0)
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;

	else if (strncmp(fourcc, "DXT5", 4) == 0)
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;

	else
	{
		printf("texture %s is dxt %u\n", path, fourcc);
		return;
		//tse_assert(false, "not dxt 1 / 3 / 5");
	}

	unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;

	if (w == 4096 || h == 4096)
	{
		//log_(path, " is a 4K texture");
	}

	for (int i = 0; i < count && (w || h); ++i)
	{
		if (w == 0 || h == 0)
		{ // 0x1 0x2 resolutions
			count--;
			// printf("discarding odd mipmap\n");
			continue;
		}
		int size = ((w + 3) / 4) * ((h + 3) / 4) * blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D, i, format, w, h, 0, size, dds->blBuffer + offset);
		offset += size;
		w /= 2;
		h /= 2;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, count - 1);

	dds_free(dds);

	// takes a long time
	//glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
}