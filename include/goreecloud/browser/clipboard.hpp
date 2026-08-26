#pragma once

#include <algorithm>
#include <cstddef>
#include <deque>
#include <string>
#include <utility>

namespace goreecloud::browser {

struct ClipboardLimits {
  std::size_t recent_entries{10};
  std::size_t pinned_entries{20};
  std::size_t maximum_entry_bytes{64 * 1024};
};

struct ClipboardEntry {
  std::string text;
  bool pinned{false};
};

class ClipboardHistory {
 public:
  explicit ClipboardHistory(ClipboardLimits limits = {}) : limits_(limits) {}

  [[nodiscard]] bool capture_browser_copy(std::string text,
                                          bool private_context) {
    if (private_context || text.empty() ||
        text.size() > limits_.maximum_entry_bytes) {
      return false;
    }

    auto it = std::find_if(recent_.begin(), recent_.end(),
                           [&](const ClipboardEntry& entry) {
                             return entry.text == text;
                           });
    if (it != recent_.end()) {
      ClipboardEntry promoted = std::move(*it);
      recent_.erase(it);
      recent_.push_front(std::move(promoted));
      return true;
    }

    recent_.push_front({std::move(text), false});
    while (recent_.size() > limits_.recent_entries) {
      recent_.pop_back();
    }
    return true;
  }

  void clear_recent() noexcept { recent_.clear(); }

  [[nodiscard]] const std::deque<ClipboardEntry>& recent() const noexcept {
    return recent_;
  }

 private:
  ClipboardLimits limits_;
  std::deque<ClipboardEntry> recent_;
};

}  // namespace goreecloud::browser
