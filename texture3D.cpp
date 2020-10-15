#include "texture3d.h"
#include <cmath>
#include <cassert>
#include "math/util.h"
#include "color.h"
#include "image.h"

namespace rendertoy {

Texture3D::Texture3D(const char* filename, bool sRGB) 
{
    int width, height, origin_channel;
    float* data = (float*)image_loadf(filename, &width, &height, &origin_channel, 4);
    assert(data);
    int face_width = width / 4;
    int face_height = height / 3;

    std::array<Vec2i, 6> face_data_offset = { {
        {face_width, face_height}, //front
        {face_width * 3, face_height}, //back
        {0, face_height}, //left
        {face_width * 2, face_height}, //right
        {face_width, face_height * 2}, //top
        {face_width, 0} //bottom
    } };

    for (int i = 0; i < face_data_offset.size(); ++i) {
        Vec2i offset = face_data_offset[i];
        Texture2D tmp(data, offset, width, face_width, face_height, origin_channel, sRGB);
        faces_[i].Swap(tmp);
    }

    image_free(data);
}

Vec3f Texture3D::SampleRGB(Vec3f coord) const {
    Vec4f color = Sample3D(coord);
    return { color.r, color.g, color.b };
}

Vec4f Texture3D::Sample3D(Vec3f coord) const {
    float x = coord.x;
    float y = coord.y;
    float z = coord.z;

    float absX = math::Abs(x);
    float absY = math::Abs(y);
    float absZ = math::Abs(z);

    float max_comp = math::Max(math::Max(absX, absY), absZ);
    float u, v;
    int index;

    if (max_comp == absX) {
        if (x > 0) { // POSITIVE X
            // u (0 to 1) goes from +z to -z
            // v (0 to 1) goes from -y to +y
            u = -z;
            v = y;
            index = static_cast<int>(CubeFace::kRight);
        } else { // NEGATIVE X
            // u (0 to 1) goes from -z to +z
            // v (0 to 1) goes from -y to +y            
            u = z;
            v = y;
            index = static_cast<int>(CubeFace::kLeft);
        }
    } else if (max_comp == absY) {
        if (y > 0) { // POSITIVE Y
            // u (0 to 1) goes from -x to +x
            // v (0 to 1) goes from +z to -z
            u = x;
            v = -z;
            index = static_cast<int>(CubeFace::kTop);
        } else { // NEGATIVE Y
            // u (0 to 1) goes from -x to +x
            // v (0 to 1) goes from -z to +z
            u = x;
            v = z;
            index = static_cast<int>(CubeFace::kBottom);
        }
    } else {
        if (z > 0) { // POSITIVE Z
            // u (0 to 1) goes from -x to +x
            // v (0 to 1) goes from -y to +y
            u = x;
            v = y;
            index = static_cast<int>(CubeFace::kFront);
        } else { // NEGATIVE Z
            // u (0 to 1) goes from +x to -x
            // v (0 to 1) goes from -y to +y
            u = -x;
            v = y;
            index = static_cast<int>(CubeFace::kBack);
        }
    }
    
    // Convert range from -1 to 1 to 0 to 1
    u = 0.5f * (u / max_comp + 1.0f);
    v = 0.5f * (v / max_comp + 1.0f);

    Vec4f color = faces_[index].Sample2D(u, v);
    return color;
}

}
