#pragma once

#include <imgui.h>

namespace gloom
{
    extern ImFont *font2;
    extern ImFont *font3;
}

void ImRotateStart();
ImVec2 ImRotationCenter();
void ImRotateEnd(float, ImVec2);