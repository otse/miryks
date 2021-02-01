#ifndef GLOOM_OBJECT_ARRAY_H
#define GLOOM_OBJECT_ARRAY_H

#include <Gloom/Dark2.h>

#include <libs>

namespace gloom
{
    // wraps espcarray

    class ObjectArray
    {
    public:
        ObjectArray(EspCArray *);

        EspCArray *const array;

        template <class UnaryFunction>
        void ForEach(int type, UnaryFunction f)
        {
            for (unsigned int i = 0; i < array->size; i++)
            {
                void *t = array->elements[i];
                if (type == 0 || ((Record *)t)->x == type)
                    f(t);
            }
        }
    };

    void gloom_object_array_example(EspCArray *);

} // namespace gloom

#endif