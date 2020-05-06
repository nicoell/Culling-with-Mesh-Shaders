# Culling with NVIDIA Mesh Shaders
This project studies the possibilites and challenges of NVIDIA Mesh Shaders. With custom meshlet based backface and frustum culling a comparison between Mesh Shader, Compute Shader and standard Vertex Shader is conducted to evaluate possible performance gains, pitfalls and obstacles.

A personal secondary purpose of the project was to implement a C++ framework from ground up using Entity Component System. The [EnTT library](https://github.com/skypjack/entt) was used for this.

## Video Analysis
[![Video coming soon](http://img.youtube.com/vi/TODO/0.jpg)](http://www.google.com)

## List of Dependendies
* Assimp (5.0.1)
* ImGui (1.74)
* EnTT (3.3.0)
* GLM (0.9.9.7)
* Glad (0.1.33)
* GLFW3 (3.3.2)

## Point of Interests
* OpenGL Shader and Buffer Setup

   [See different Test Scenes](../../tree/master/src/nell/scenes)
* Meshlet, Bounding Sphere and Bounding Cone creation

   [Meshlet Triangle Mesh Header](../master/include/nell/components/meshlet_triangle_mesh.hpp)
   
   [Meshlet Triangle Mesh CPP](../master/src/nell/components/meshlet_triangle_mesh.cpp)
* Main Loop
   
   [See Context::run()](../master/src/nell/context.cpp)
* Shaders

   [See Resources/Shaders/](../../tree/master/resources/shaders)
