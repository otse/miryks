#include <string>

namespace gloom
{
	class Image
	{
	public:
		Image() {};
		~Image() {};
		void from_resourcefile();
		const unsigned char *data, *cursor;
		unsigned int size;
	};
}