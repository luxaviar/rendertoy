#pragma once

#include <array>
#include "math/vec2.h"
#include "math/vec3.h"
#include "math/vec4.h"
#include "uncopyable.h"

namespace rendertoy {

struct Triangle {
    //Texture *tex;
    Triangle();

    void SetVertex(int idx, float x, float y, float z);
    void SetVertex(int idx, const Vec3f& ver);
    void SetVertex(int idx, const Vec4f& ver);

    void SetNormal(int idx, const Vec3f& n);

    void SetColor(int idx, float r, float g, float b);
    void SetColor(int idx, const Vec3f& col);
    void SetColor(int idx, const Vec4f& col);

    Vec3f GetColor() const { return color[0]; } // Only one color per triangle.

    void SetTexCoord(int idx, float u, float v); /*set i-th vertex texture coordinate*/

    std::array<Vec4f, 3> ToVector4() const;
    float Area2() const;

    Vec3f v[3]; /*the original coordinates of the triangle, v0, v1, v2 in clockwise order*/
    Vec3f color[3]; //color at each vertex;
    Vec2f tex_coords[3]; //texture u,v
    Vec3f normal[3]; //normal vector for each vertex
};

}
