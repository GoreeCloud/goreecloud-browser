#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace goreecloud::browser {

enum class MemoryProfile {
  balanced,
  performance,
  memory_saver,
};

enum class MemoryReclamationStage {
  cache_reclamation,
  background_freezing,
  background_unloading,
};

struct TabActivitySignals {
  bool foreground{false};
  bool active_download{false};
  bool unsaved_work{false};
  bool active_audio{false};
  bool active_video{false};
  bool picture_in_picture{false};
  bool webrtc{false};
  bool pinned{false};
  bool explicitly_protected{false};
  std::uint64_t estimated_memory_bytes{};
  std::uint64_t last_used_monotonic_tick{};
};

struct MemoryDecision {
  std::string tab_id;
  MemoryReclamationStage stage{MemoryReclamationStage::cache_reclamation};
  std::string explanation;
};

class MemoryManager {
 public:
  virtual ~MemoryManager() = default;
  [[nodiscard]] virtual MemoryProfile profile() const = 0;
  virtual void set_profile(MemoryProfile profile) = 0;
  [[nodiscard]] virtual std::vector<MemoryDecision> plan_reclamation() const = 0;
};

}  // namespace goreecloud::browser
