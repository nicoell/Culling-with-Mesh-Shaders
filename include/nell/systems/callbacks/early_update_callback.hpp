#pragma once

namespace nell
{
class EarlyUpdateCallback
{
 public:
  virtual ~EarlyUpdateCallback() = default;
  virtual void earlyUpdate() = 0;
};

}  // namespace nell