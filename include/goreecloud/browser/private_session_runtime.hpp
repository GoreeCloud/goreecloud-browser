#pragma once

#include <string>

#include "goreecloud/browser/application.hpp"
#include "goreecloud/browser/private_browsing.hpp"

namespace goreecloud::browser {

class PrivateSessionRuntime {
 public:
  PrivateSessionRuntime(BrowserApplication& application,
                        PrivateBrowsingManager& manager)
      : application_(application), manager_(manager) {}

  bool close_and_forget(const std::string& session_id) {
    if (!manager_.close_and_forget_session(session_id)) return false;
    return application_.destroy_private_session_context(session_id);
  }

  bool clear_everything(const std::string& session_id) {
    return manager_.clear_everything(session_id);
  }

  bool lock(const std::string& session_id) {
    return manager_.lock_session(session_id);
  }

  bool unlock(const std::string& session_id) {
    return manager_.unlock_session(session_id);
  }

 private:
  BrowserApplication& application_;
  PrivateBrowsingManager& manager_;
};

}  // namespace goreecloud::browser
