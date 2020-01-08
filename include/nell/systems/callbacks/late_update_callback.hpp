#pragma once

namespace nell
{
class LateUpdateCallback
{
 public:
  virtual ~LateUpdateCallback() = default;
  virtual void lateUpdate() = 0;
};

}  // namespace nell