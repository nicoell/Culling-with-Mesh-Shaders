#pragma once

namespace nell
{
class PostRenderCallback
{
 public:
  virtual ~PostRenderCallback() = default;
  virtual void postRender() = 0;
};

}  // namespace nell