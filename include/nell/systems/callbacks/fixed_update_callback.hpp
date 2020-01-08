#pragma once

namespace nell
{
class FixedUpdateCallback
{
 public:
  virtual ~FixedUpdateCallback() = default;
  virtual void fixedUpdate() = 0;
};

}  // namespace nell