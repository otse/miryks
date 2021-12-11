#include <miryks/reference.h>

#include <vector>
#include <algorithm>

namespace miryks
{
namespace itemfinder
{
bool detect = true;
reference *handRef = nullptr;
std::vector<reference *> pool;
vec3 projected;
void init()
{
}
void consider()
{
    if (!detect)
        return;
    std::sort(pool.begin(), pool.end(), [](const reference *l, const reference *r) -> bool {
        return l->GetDistance() < r->GetDistance();
    });
    handRef = nullptr;
    for (reference *refer : pool)
        if (refer->DisplayAsItem())
            return;
}
void activate() {
    if (handRef)
        handRef->Use();
}
}
}