#pragma once

#include <string>
#include <vector>
#include <imgui.h>

namespace dark
{
    extern ImFont *font2;
    extern ImFont *font3;
}

void ImRotateStart();
ImVec2 ImRotationCenter();
void ImRotateEnd(float, ImVec2);