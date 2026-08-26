#pragma once

#include <cstdint>

namespace goreecloud::browser {

// Opaque platform surface description used to attach an engine-rendered view
// to GoreeCloud-owned native chrome without exposing Chromium types upstream.
struct NativeEngineSurface {
  std::uintptr_t window_handle{0};
  std::uintptr_t display_handle{0};
  int x{0};
  int y{0};
  int width{0};
  int height{0};
  float scale_factor{1.0F};
};

class NativeSurfaceAttachable {
 public:
  virtual ~NativeSurfaceAttachable() = default;
  virtual bool attach_native_surface(const NativeEngineSurface& surface) = 0;
  virtual void resize_native_surface(const NativeEngineSurface& surface) = 0;
  virtual void detach_native_surface() = 0;
  [[nodiscard]] virtual bool native_surface_attached() const noexcept = 0;
};

}  // namespace goreecloud::browser
