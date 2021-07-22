#pragma once

#include <core/files.h>

#include <resourcefile.h>

#include <zlib.h>

#define RSF_EXT "" // .rsf ?

std::map<std::string, Resourcefile *> resourcefiles;

// depends on microtar

// offshoring a file means searching in a folder then the tar

static void example() 
{
	resourcefile_offshore("foo", "bar.ext");
	resourcefile_offshore("foo", "bar", ".ext", ".ext.gz");
}

std::string resourcefile_offshore(const std::string &a, const std::string &b)
{
	return resourcefile_offshore(a, b, "", "");
}

std::string resourcefile_offshore(
	const std::string &a,
	const std::string &b,
	const std::string &c,
	const std::string &d)
{
	if (exists_test3(a + "/" + b + c))
		return fread(a + "/" + b + c);
	if (resourcefile_find(a, "./" + b + d))
		return resourcefile_read(a, "./" + b + d);
	return "No";
}

void _resourcefile_makeindex(Resourcefile &rsf)
{
	rsf.filenames.clear();
	mtar_header_t h;
	while ((mtar_read_header(&rsf.mtar, &h)) != MTAR_ENULLRECORD)
	{
		rsf.filenames.push_back(h.name);
		rsf.headers_by_name.emplace(h.name, h);
		mtar_next(&rsf.mtar);
	}
}

Resourcefile &resourcefile_handle(std::string a)
{
	auto has = resourcefiles.find(a);
	if (resourcefiles.end() != has)
		return *has->second;
	else
	{
		Resourcefile &rsf = *(new Resourcefile{a});
		resourcefiles.emplace(a, &rsf);
		printf("resourcefile: %s\n", a.c_str());
		a += RSF_EXT;
		int err = mtar_open(&rsf.mtar, a.c_str(), "r");
		if (err)
			printf(mtar_strerror(err));
		_resourcefile_makeindex(rsf);
		return rsf;
	}
}

bool resourcefile_find(const std::string &a, std::string path)
{
	Resourcefile &rsf = resourcefile_handle(a);
	auto has = std::find(rsf.filenames.begin(), rsf.filenames.end(), path);
	return has != rsf.filenames.end();
}

const mtar_header_t &get_header(Resourcefile &rsf, std::string path)
{
	const mtar_header_t &h = rsf.headers_by_name[path.c_str()];
	return h;
}

static inline std::string tinflate(const std::string & deflate)
{
	unsigned int len = *((unsigned int *)&deflate[deflate.size() - 4]);
	std::string inflate(len, ' ');
	zinf(deflate.data(), deflate.size(), inflate.data(), len);
	return inflate;
}

std::string resourcefile_read(std::string a, std::string path)
{
	Resourcefile &rsf = resourcefile_handle(a);
	mtar_header_t h;
	if (!resourcefile_find(a, path))
		return "No";
	get_header(rsf, path);
	std::string str(h.size, ' ');
	mtar_read_data(&rsf.mtar, str.data(), h.size);
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