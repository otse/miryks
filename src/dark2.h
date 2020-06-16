#ifndef DARK2_H
#define DARK2_H

struct bsa_t;

class camera_t;

#include "idiom.h"

namespace dark2
{
    void program_loop();

    extern int width, height;
    extern float delta;

    extern string SKYRIM_PATH;

    extern camera_t *camera;

    extern bsa_t interface;
    extern bsa_t meshes;
    extern bsa_t animations;
}

#endif