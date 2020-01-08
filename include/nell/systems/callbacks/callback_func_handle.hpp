#pragma once

template <typename T>
struct CallbackFuncHandle
{
  CallbackFuncHandle(unsigned handle, T &callback)
      : handle(handle), callback(callback)
  {
  }

  unsigned handle;
  T &callback;
};