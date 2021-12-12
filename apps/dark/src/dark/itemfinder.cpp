#if 0
#include <dark/dark.h>
#include <dark/reference.h>

#include <miryks/miryks.hpp>

#include <vector>
#include <algorithm>

namespace miryks
{
    namespace itemfinder
    {
        bool detect = true;
        my_reference *handRef = nullptr;
        std::vector<my_reference *> pool;
        vec3 projected;
        void init()
        {
        }
        void consider()
        {
            if (!detect)
                return;
            std::sort(pool.begin(), pool.end(), [](const reference *l, const reference *r) -> bool
                      { return l->GetDistance() < r->GetDistance(); });
            handRef = nullptr;
            for (reference *refer : pool)
                if (refer->DisplayAsItem())
                    return;
        }
        void activate()
        {
            if (handRef)
                handRef->Use();
        }
    }
}
#endif