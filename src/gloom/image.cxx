#include <Gloom/Image.h>

#include <OpenGL/Types.h>

#include <string>

#include <png.h>
#include <resourcefile.h>

// adapted from github.com/ileben/GameEngine/blob/master/src/engine/image/png_read.cpp

namespace gloom
{
	void callback(png_struct *png_ptr, png_byte *data, png_size_t length)
	{
		auto image = (gloom::Image *)png_get_io_ptr(png_ptr);
		size_t bytes_read = length;
		size_t bytes_till_end = image->data + image->size - image->cursor;
		if (bytes_read > bytes_till_end)
			bytes_read = bytes_till_end;
		printf("callback bytes_read: %i\n", bytes_read);
		memcpy(data, image->cursor, bytes_read);
		image->cursor += bytes_read;
		if (bytes_read != length)
			png_error(png_ptr, "Read Error!");
	}

	void Image::from_resourcefile()
	{
		std::string yagrum = resourcefile_offshore("img", "yagrum.png");
		cursor = 0;
		data = (const unsigned char *)yagrum.data();
		cursor = data;
#if 1
		cursor += 8;
#endif
		size = yagrum.length();
		unsigned char header[8];
		memcpy(header, data, 8);
		if (png_sig_cmp(header, 0, 8))
			printf("[read_png_file] File is not recognized as a PNG file");
		printf("woo yagrum!");
		png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		png_infop info_ptr = png_create_info_struct(png_ptr);
		setjmp(png_jmpbuf(png_ptr));
		png_set_read_fn(png_ptr, this, callback);
		//png_init_io(png_ptr, yagrum.data());
		png_set_sig_bytes(png_ptr, 8);
		png_read_info(png_ptr, info_ptr);
		int number_of_passes = png_set_interlace_handling(png_ptr);
	}
}