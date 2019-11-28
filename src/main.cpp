// ----------------------------------------------------------------------------------
//  Culling with Mesh Shaders
//  File: main.cpp
//  Author: Nico Ell
//  Email: cvnicoell@gmail.com
//
//  Copyright (c) 2019, Nico. All rights reserved.
// ----------------------------------------------------------------------------------

#include <nell/context.hpp>

int main(int argc, char *argv[])
{
  auto context_options = nell::ContextOptions{1280, 720, "nell Viewer", "test"};
  auto context = nell::Context(context_options);

  context.run();
}
