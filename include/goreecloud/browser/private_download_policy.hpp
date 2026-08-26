#pragma once

#include <optional>
#include <string>
#include <unordered_map>

#include "goreecloud/browser/private_browsing.hpp"

namespace goreecloud::browser {

enum class PrivateDownloadDisposition {
  undecided,
  keep_file,
  delete_on_session_close,
};

struct PrivateDownloadRecord {
  std::string download_id;
  std::string private_session_id;
  PrivateDownloadDisposition disposition{PrivateDownloadDisposition::undecided};
  bool wardveil_checked{false};
  bool completed{false};
  bool file_exists{false};
};

class PrivateDownloadPolicyController {
 public:
  void register_download(PrivateDownloadRecord record) {
    records_[record.download_id] = std::move(record);
  }

  bool choose_disposition(const std::string& download_id,
                          PrivateDownloadDisposition disposition) {
    auto it = records_.find(download_id);
    if (it == records_.end()) return false;
    it->second.disposition = disposition;
    return true;
  }

  [[nodiscard]] bool may_report_safe_completion(const std::string& download_id) const {
    const auto it = records_.find(download_id);
    return it != records_.end() && it->second.completed && it->second.wardveil_checked;
  }

  template <typename DeleteFileFn>
  bool close_session(const std::string& session_id, DeleteFileFn&& delete_file) {
    bool ok = true;
    for (auto it = records_.begin(); it != records_.end();) {
      if (it->second.private_session_id != session_id) {
        ++it;
        continue;
      }
      if (it->second.file_exists &&
          it->second.disposition == PrivateDownloadDisposition::delete_on_session_close) {
        ok = delete_file(it->second.download_id) && ok;
      }
      it = records_.erase(it); // Browser history record is always temporary.
    }
    return ok;
  }

 private:
  std::unordered_map<std::string, PrivateDownloadRecord> records_;
};

}  // namespace goreecloud::browser
