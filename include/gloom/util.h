#ifndef GLOOM_UTIL_H
#define GLOOM_UTIL_H

// part of gloom

#include <libs>
#include <Gloom/Dark2.h>

namespace gloom
{
	namespace util
	{
		template <typename F>
		void GrupEach(F &lambda)
		{
			bool stop = false;
			//Objects(grup).ForEach(0, stop, lambda);
		}

		Mesh *GrantMesh(Object &baseObject);
	}

} // namespace gloom

#endif