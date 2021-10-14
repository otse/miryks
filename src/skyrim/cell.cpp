#include <skyrim/cell.h>

Cell::Cell(record cell, grup<> grup) : record(cell)
{
    assertc(grup.hed().group_type == CellChildren);
    auto first = grup.grup();
    auto second = grup.grup();
    if (first.hed().group_type == CellPersistentChildren)
    {
        persistent = first;
        temporary = second;
    }
    else
        temporary = first;
    flags = *cell.data<uint16_t *>("DATA");
}