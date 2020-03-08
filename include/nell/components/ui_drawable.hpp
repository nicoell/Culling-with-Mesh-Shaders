#pragma once
#include <imgui/misc/imgui_stdlib.h>

struct UiDrawable
{
  virtual ~UiDrawable () = default;
  virtual void drawImGui() = 0;
};