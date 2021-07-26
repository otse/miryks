#pragma once

#include <zlib.h>

#include "files.h"
#include "basefile.h"

#define RSF_EXT "" // .rsf ?

std::map<std::string, Basefile *> resourcefiles;

// depends on microtar

// offshoring a file means searching in a folder then the tar

static void example()
{
	basefile_offshore("foo", "bar.ext");
	basefile_offshore("foo", "bar", ".ext", ".ext.gz");
}

std::string basefile_offshore(const std::string &a, const std::string &b)
{
	return basefile_offshore(a, b, "", "");
}

std::string basefile_offshore(
	const std::string &a,
	const std::string &b,
	const std::string &ext,
	const std::string &ext2)
{
	if (exists_test3(a + "/" + b + ext))
		return fread(a + "/" + b + ext);
	if (basefile_find(a, b + ext2))
		return basefile_read(a, b + ext2);
	return "";
}

void _basefile_makeindex(Basefile &rsf)
{
	rsf.fnam.clear();
	mtar_header_t h;
	while ((mtar_read_header(&rsf.mtar, &h)) != MTAR_ENULLRECORD)
	{
		rsf.fnam.push_back(h.name);
		// printf("rsf make index %s of size %u\n", h.name, h.size);
		rsf.hnam.emplace(h.name, h);
		mtar_next(&rsf.mtar);
	}
}

Basefile &basefile_handle(std::string a)
{
	auto has = resourcefiles.find(a);
	if (resourcefiles.end() != has)
		return *has->second;
	else
	{
		Basefile &rsf = *(new Basefile{a});
		resourcefiles.emplace(a, &rsf);
		// printf("basefile: %s\n", a.c_str());
		a += RSF_EXT;
		int err = mtar_open(&rsf.mtar, a.c_str(), "r");
		if (err)
			printf(mtar_strerror(err));
		_basefile_makeindex(rsf);
		return rsf;
	}
}

bool basefile_find(const std::string &a, std::string path)
{
	Basefile &rsf = basefile_handle(a);
	auto has = std::find(rsf.fnam.begin(), rsf.fnam.end(), path);
	return has != rsf.fnam.end();
}

const mtar_header_t get_header(Basefile &rsf, std::string path)
{
	auto it = rsf.hnam.find(path);
	if (it == rsf.hnam.end())
		printf("rsf bad hnam\n");
	const mtar_header_t h = rsf.hnam[path];
	return h;
}

static inline std::string tinflate(const std::string &deflate)
{
	unsigned int len = *((unsigned int *)&deflate[deflate.size() - 4]);
	std::string inflate(len, ' ');
	zinf(deflate.data(), deflate.size(), inflate.data(), len);
	return inflate;
}

std::string basefile_read(std::string a, std::string path)
{
	Basefile &rsf = basefile_handle(a);
	mtar_header_t h;
	if (!basefile_find(a, path))
		return "";
	mtar_find(&rsf.mtar, path.c_str(), &h);
	std::string str(h.size, ' ');
	int err = mtar_read_data(&rsf.mtar, str.data(), h.size);
	if (err)
		printf(mtar_strerror(err));
	if (path.find(".gz") != std::string::npos)
		str = tinflate(str);
	return str;
}

static int zinf(const void *src, int srcLen, void *dst, int dstLen)
{
	z_stream strm = {0};
	strm.total_in = strm.avail_in = srcLen;
	strm.total_out = strm.avail_out = dstLen;
	strm.next_in = (Bytef *)src;
	strm.next_out = (Bytef *)dst;

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;

	int err = -1;
	int ret = -1;

	err = inflateInit2(&strm, (15 + 32)); //15 window bits, and the +32 tells zlib to to detect if using gzip or zlib
	if (err == Z_OK)
	{
		err = inflate(&strm, Z_FINISH);
		if (err == Z_STREAM_END)
		{
			ret = strm.total_out;
		}
		else
		{
			inflateEnd(&strm);
			return err;
		}
	}
	else
	{
		inflateEnd(&strm);
		return err;
	}

	inflateEnd(&strm);
	return ret;
}