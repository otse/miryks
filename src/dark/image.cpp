#include <string>

#include <png.h>

#include <dark/image.h>
#include <dark/files.h>

#include <renderer/renderer.h>

// adapted from github.com/ileben/GameEngine/blob/master/src/engine/image/png_read.cpp

namespace dark
{
	void callback(png_struct *png_ptr, png_byte *data, png_size_t length)
	{
		auto image = (dark::Image *)png_get_io_ptr(png_ptr);
		size_t bytes_read = length;
		size_t bytes_till_end = image->data + image->size - image->cursor;
		if (bytes_read > bytes_till_end)
			bytes_read = bytes_till_end;
		memcpy(data, image->cursor, bytes_read);
		image->cursor += bytes_read;
		if (bytes_read != length)
			png_error(png_ptr, "Read Error!");
	}

	void Image::from_resourcefile()
	{
		char *buf;
		int end = fbuf(path, &buf);
		cursor = 0;
		data = (const unsigned char *)buf;
		cursor = data;
		cursor += 8;
		size = end;
		unsigned char header[8];
		memcpy(header, data, 8);
		if (png_sig_cmp(header, 0, 8))
			printf("[read_png_file] File is not recognized as a PNG file");
		png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		png_infop info_ptr = png_create_info_struct(png_ptr);
		setjmp(png_jmpbuf(png_ptr));
		png_set_read_fn(png_ptr, this, callback);
		// If we have already read some of the signature
		png_set_sig_bytes(png_ptr, 8);
		png_read_info(png_ptr, info_ptr);
		int number_of_passes = png_set_interlace_handling(png_ptr);
		width = png_get_image_width(png_ptr, info_ptr);
		height = png_get_image_height(png_ptr, info_ptr);
		color_type = png_get_color_type(png_ptr, info_ptr);
		bit_depth = png_get_bit_depth(png_ptr, info_ptr);
		printf("image is %i %i\n", width, height);
		copy_pixels(png_ptr, info_ptr, &pixels);
		// if (color_type == PNG_COLOR_TYPE_RGB)
		// 	printf("color_type == PNG_COLOR_TYPE_RGB\n");
		// if (color_type == PNG_COLOR_TYPE_RGBA)
		// 	printf("color_type == PNG_COLOR_TYPE_RGBA\n");
		create_texture();
	}

	void Image::copy_pixels(png_structp png_ptr, png_infop info_ptr, GLubyte **dst)
	{
		unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);

		png_bytepp row_pointers = NULL;
		png_bytep image_data = NULL;

		image_data = (unsigned char *)malloc(row_bytes * height);
		row_pointers = (png_bytepp)malloc(height * sizeof(png_bytep));

		for (int i = 0; i < height; ++i)
			row_pointers[i] = image_data + i * row_bytes;

		png_read_image(png_ptr, row_pointers);

		*dst = (unsigned char *)malloc(row_bytes * height);

		for (int i = 0; i < height; i++)
			memcpy(*dst + (row_bytes * i), row_pointers[i], row_bytes);

		// printf("copied pixels\n");
	}

	void Image::create_texture()
	{
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		if (pixels)
		{
			auto mode = color_type == PNG_COLOR_TYPE_RGBA ? GL_RGBA : GL_RGB;
			glTexImage2D(GL_TEXTURE_2D, 0, mode, width, height, 0, mode, GL_UNSIGNED_BYTE, pixels);
		}
	}
}