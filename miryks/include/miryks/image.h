#pragma once

#include <string>

#include <png.h>

namespace miryks
{
	class Image
	{
	public:
		Image(){};
		~Image(){};
		void copy_pixels(png_structp, png_infop, unsigned char **);
		void fromfile(const char *);
		void create_texture();
		const unsigned char *data, *cursor;
		unsigned int size;
		int width, height;
		png_byte color_type;
		png_byte bit_depth;
		unsigned int texture;
		unsigned char *pixels = nullptr;
	};
}