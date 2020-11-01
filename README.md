# Rendertoy

Yet another soft renderer for learning

## Prerequistes
* C++ compiler with support for the C++17 standard
* CMake, version 3.10 or higher

## Features

* Shader-based
* Homogeneous clipping(based on SutherlandHodgeman algorithm)
* Back/Front face culling
* Point light and directional light
* Normal mapping
* Cubemap and skybox
* Blinn-Phong shading
* Physically based rendering(metalness workflow)
* Image-based Lighting
* HDR/linear lighting
* tone mappers: ACES
* MSAA(2x/4x)
* Shadow(based on shadow map)

## Example

wireframe

![PBR](samples/output_wireframe.png)

blinn-phong

![PBR](samples/output_0.png)

pbr

![PBR](samples/output_1.png)

![PBR](samples/output_2.png)

msaax4

![PBR](samples/output_msaax4.png)

shadow

![PBR](samples/output_shadow.png)

![PBR](samples/shadow_map.png)
