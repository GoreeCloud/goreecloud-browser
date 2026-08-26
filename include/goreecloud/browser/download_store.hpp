#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "goreecloud/browser/download.hpp"

namespace goreecloud::browser {

struct DownloadStoreSnapshot {
  std::vector<DownloadRecord> records;
  std::uint64_t schema_version{1};
};

class DownloadStore {
 public:
  virtual ~DownloadStore() = default;
  virtual bool upsert(const DownloadRecord& record) = 0;
  virtual bool remove(std::string_view id) = 0;
  [[nodiscard]] virtual std::optional<DownloadRecord> get(
      std::string_view id) const = 0;
  [[nodiscard]] virtual std::vector<DownloadRecord> list() const = 0;
  [[nodiscard]] virtual bool save_snapshot(const DownloadStoreSnapshot& snapshot) = 0;
  [[nodiscard]] virtual std::optional<DownloadStoreSnapshot> load_snapshot() = 0;
};

}  // namespace goreecloud::browser
