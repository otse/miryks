#include <gloom/dark2.h>

namespace gloom
{
    vec2 *cast_vec_2(float *f) { return reinterpret_cast<vec2 *>(f); }
    vec3 *cast_vec_3(float *f) { return reinterpret_cast<vec3 *>(f); }
    vec4 *cast_vec_4(float *f) { return reinterpret_cast<vec4 *>(f); }
    mat3 *cast_mat_3(float *f) { return reinterpret_cast<mat3 *>(f); }
    mat4 *cast_mat_4(float *f) { return reinterpret_cast<mat4 *>(f); }
    
} // namespace gloom