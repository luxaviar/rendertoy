cmake_minimum_required(VERSION 3.10)

project(RenderToy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)

set(MATERIAL_SRC 
    src/material/material.cpp 
    src/material/vertlit_material.cpp 
    src/material/blinnphong_material.cpp 
    src/material/normal_material.cpp 
    src/material/pbr_material.cpp
    src/material/skybox_material.cpp
)

set(SHADER_SRC
    src/shader/shader.cpp 
    src/shader/vertlit_shader.cpp 
    src/shader/blinnphong_shader.cpp 
    src/shader/normal_shader.cpp 
    src/shader/pbr_shader.cpp 
    src/shader/skybox_shader.cpp
    src/shader/shadow_shader.cpp
)

set(MAIN_SRC
    src/pipeline.cpp 
    src/mesh.cpp
    src/rendertexture.cpp 
    src/model.cpp 
    src/camera.cpp 
    src/graphics.cpp 
    src/image.cpp 
    src/texture2D.cpp 
    src/texture3D.cpp
    src/light.cpp
)

add_executable(render src/example/pbr.cpp ${MAIN_SRC} ${SHADER_SRC} ${MATERIAL_SRC})
add_executable(render_blinn_phong src/example/blinn_phong.cpp ${MAIN_SRC} ${SHADER_SRC} ${MATERIAL_SRC})
add_executable(render_shadow src/example/shadow.cpp ${MAIN_SRC} ${SHADER_SRC} ${MATERIAL_SRC})
