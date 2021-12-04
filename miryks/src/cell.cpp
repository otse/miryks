#include <miryks/cell.h>

namespace miryks
{
    Cell::Cell(record &cell, grup<> &g) : Record(cell)
    {
        assertc(g.hed().group_type == cell_children);
        printf("get nexts\n");

        grup<> first, second;

        first = g.next_grup();
        second = g.next_grup();

        if (first.hed().group_type == cell_persistent_children)
        {
            printf("set persistent and temporary\n");
            persistent = first;
            temporary = second;
        }
        else
            temporary = first;
        flags = *cell.data<uint16_t *>("DATA");
    }
}