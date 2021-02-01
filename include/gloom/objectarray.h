#ifndef GLOOM_OBJECT_ARRAY_H
#define GLOOM_OBJECT_ARRAY_H

#include <Gloom/Dark2.h>

#include <libs>

namespace gloom
{
    // wraps espcarray, see the example

    void gloom_object_array_example(EspCArray *);

    class ObjectArray
    {
    public:
        EspCArray *const array;

        ObjectArray(EspCArray *);

        template <class UnaryFunction>
        void ForEach(int type, UnaryFunction f)
        {
            for (size_t i = 0; i < array->size; i++)
            {
                void *t = array->elements[i];
                if (type == 0 || ((TypeDud *)t)->x == type)
                    f(t, i);
            }
        }
    };

} // namespace gloom

#endif