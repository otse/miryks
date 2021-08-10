
#include "dds.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ffread(a, b) memcpy(a, buf + pos, b); pos += b;

DDSFile* ddsloader_load_buf(const unsigned char* buf, int size) {
	if (!buf)
		return 0;
	// this can happen

	//printf("ddsloader load buf\n");
	unsigned int isDx10;
	
	DDSFile* file = 0;

	int pos = 0;
	
	unsigned char *filesig = malloc(4);
	ffread(filesig, 4);

	//printf("filesig %s\n", filesig);
	
	if(memcmp(filesig, "DDS ", 4) != 0)
		goto exit; // not a dds file
	
	file = malloc(sizeof(DDSFile));
	if(file == 0)
		goto exit;
		
	memset(file, 0, sizeof(DDSFile));
	ffread(file, 124); // read into struct up to dwReserved2; (EOF header)
	
	//printf("dds width %i height %i\n", file->dwWidth, file->dwHeight);

	isDx10 = memcmp(&file->ddspf.dwFourCC, "DX10", 4) == 0 ? 1 : 0;
	if(isDx10) {
		//printf("its dx10\n");
		file->ddsHeaderDx10 = malloc(sizeof(DDS_HEADER_DXT10));
		if(file->ddsHeaderDx10 == 0) {
			dds_free(file);
			goto exit;
		}
		ffread(file->ddsHeaderDx10, sizeof(DDS_HEADER_DXT10));
	}
	
	file->dwFileSize = size;
	file->dwBufferSize = ((size - 124) - 4) - (isDx10 ? sizeof(DDS_HEADER_DXT10) : 0);
	
	file->blBuffer = malloc(file->dwBufferSize);
	if(file->blBuffer == 0) {
		//printf("blBuffer 0\n");
		dds_free(file);
		goto exit;
	}
	//printf("read dwBufferSize %i\n", file->dwBufferSize);
	ffread(file->blBuffer, file->dwBufferSize);
	
exit:
	free(filesig);
	return file;
}

DDSFile* ddsloader_load(const char* path) {
	unsigned int file_size;
	
	unsigned char* filesig = 0;
	DDSFile* file = 0;
	
	unsigned int isDx10;
	
	FILE* f;
	if((f = fopen(path, "rb")) == 0)
		return 0;
	fseek(f, 0, SEEK_END);
	file_size = ftell(f);
	fseek(f, 0, SEEK_SET);
	
	filesig = malloc(4);
	if(fread(filesig, 1, 4, f) != 4)
		goto exit; // empty file
	
	if(memcmp(filesig, "DDS ", 4) != 0)
		goto exit; // not a dds file
	
	file = malloc(sizeof(DDSFile));
	if(file == 0)
		goto exit;
	memset(file, 0, sizeof(DDSFile));
	fread(file, 1, 124, f); // read into struct up to dwReserved2; (EOF header)
	
	isDx10 = memcmp(&file->ddspf.dwFourCC, "DX10", 4) == 0 ? 1 : 0;
	if(isDx10) {
		file->ddsHeaderDx10 = malloc(sizeof(DDS_HEADER_DXT10));
		if(file->ddsHeaderDx10 == 0) {
			dds_free(file);
			goto exit;
		}
		fread(file->ddsHeaderDx10, sizeof(DDS_HEADER_DXT10), 1, f);
	}
	
	file->dwFileSize = file_size;
	file->dwBufferSize = ((file_size - 124) - 4) - (isDx10 ? sizeof(DDS_HEADER_DXT10) : 0);
	
	file->blBuffer = malloc(file->dwBufferSize);
	if(file->blBuffer == 0) {
		dds_free(file);
		goto exit;
	}
	fread(file->blBuffer, file->dwBufferSize, 1, f);
	
exit:
	free(filesig);
	fclose(f);
	return file;
}

void dds_free(DDSFile* file) {
	free(file->ddsHeaderDx10);
	free(file->blBuffer);
	free(file);
}

