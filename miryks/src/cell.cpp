#include <miryks/cell.h>

namespace miryks
{
    Cell::Cell(record_and_grup rng) : Record(rng)
    {
        assertc(rng.ghed().group_type == cell_children);
        printf("get nexts\n");

        grup_copy first, second;

        first = rng.next_grup();
        second = rng.next_grup();

        if (first.ghed().group_type == cell_persistent_children)
        {
            printf("set persistent and temporary\n");
            persistent = first;
            temporary = second;
        }
        else
            temporary = first;
        flags = *rng.data<uint16_t *>("DATA");
    }
}