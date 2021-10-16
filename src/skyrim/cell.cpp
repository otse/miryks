#include <skyrim/cell.h>

Cell::Cell(record cell, grup<> g) : Record(cell)
{
    assertc(g.hed().group_type == CellChildren);
    grup<> first = g.next_grup();
    grup<> second = g.next_grup();
    if (first.hed().group_type == CellPersistentChildren)
    {
        persistent = first;
        temporary = second;
    }
    else
        temporary = first;
    flags = *cell.data<uint16_t *>("DATA");
}