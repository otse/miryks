#include "camera.h"

#include "dark2.h"

const float PI = pi<float>();

camera_t::camera_t()
{
}

void camera_t::mouse(float x, float y)
{
    const float sens = .001f;

    fyaw += x * sens;
    fpitch -= y * sens;
}

void camera_t::call()
{
    while (fyaw > 2 * PI)
        fyaw -= 2 * PI;
    while (fyaw < 0)
        fyaw += 2 * PI;

    fpitch = fmaxf(-PI, fminf(0, fpitch));
    //fpitch = std::clamp(fpitch, -PI, 0.f);

    view = mat4(1.0f);
    view = rotate(view, fpitch, vec3(1, 0, 0));
    view = rotate(view, fyaw, vec3(0, 0, 1));
    view = translate(view, -pos - eye);
    //view = inverse(view);

    using namespace dark2;

    projection = perspective(
        radians(fzoom), (float)width / (float)height, 0.1f, 10000.0f);
}

void camera_t::up_down(const int dir, float time)
{
    float speed = 250 * time;
    if (shift)
        speed /= 10;

    if (r)
        pos.z += speed;
    if (f)
        pos.z -= speed;
}

void camera_t::move(float time)
{
    auto forward = [&](float n) {
        pos.x += n * sin(fyaw);
        pos.y += n * cos(fyaw);
    };
    auto strafe = [&](float n) {
        pos.x += n * cos(-fyaw);
        pos.y += n * sin(-fyaw);
    };

    float speed = 500 * time;

    if (shift)
        speed /= 10;

    if (w || !s)
        forward(speed);
    if (s || !w)
        forward(-speed / 2);

    if (a || !d)
        strafe(-speed);
    if (d || !a)
        strafe(speed);

    //tSay("pos ", pos.x, ", ", pos.y);
}