cmake_minimum_required(VERSION 3.10)
project(RenderToy)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(render main.cpp pipeline.cpp mesh.cpp rendertexture.cpp 
model.cpp camera.cpp graphics.cpp image.cpp texture2D.cpp texture3D.cpp
shader/vertlit_shader.cpp shader/blinnphong_shader.cpp shader/normal_shader.cpp shader/pbr_shader.cpp shader/skybox_shader.cpp
material/material.cpp material/vertlit_material.cpp material/blinnphong_material.cpp material/normal_material.cpp material/pbr_material.cpp
material/skybox_material.cpp
)
