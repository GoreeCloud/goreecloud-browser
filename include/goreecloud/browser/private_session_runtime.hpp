#pragma once

#include <string>

#include "goreecloud/browser/application.hpp"
#include "goreecloud/browser/private_browsing.hpp"
#include "goreecloud/browser/private_cleanup_controller.hpp"

namespace goreecloud::browser {

class PrivateSessionRuntime {
 public:
  PrivateSessionRuntime(BrowserApplication& application,
                        PrivateBrowsingManager& manager)
      : application_(application), manager_(manager), cleanup_(application) {}

  bool forget_site(const std::string& session_id, const std::string& origin) {
    if (!cleanup_.forget_origin(session_id, origin)) return false;
    return manager_.forget_site(session_id, origin);
  }

  bool clear_cookies(const std::string& session_id) {
    return cleanup_.clear_cookies(session_id) && manager_.clear_cookies(session_id);
  }

  bool clear_cache(const std::string& session_id) {
    return cleanup_.clear_cache(session_id) && manager_.clear_cache(session_id);
  }

  bool clear_site_storage(const std::string& session_id) {
    return cleanup_.clear_storage(session_id) && manager_.clear_site_storage(session_id);
  }

  bool clear_authentication(const std::string& session_id) {
    return cleanup_.clear_authentication(session_id) && manager_.clear_authentication(session_id);
  }

  bool clear_permissions(const std::string& session_id) {
    return cleanup_.clear_permissions(session_id) && manager_.clear_permissions(session_id);
  }

  bool clear_history(const std::string& session_id) {
    return cleanup_.clear_history(session_id) && manager_.clear_history(session_id);
  }

  bool clear_everything(const std::string& session_id) {
    return cleanup_.clear_everything(session_id) && manager_.clear_everything(session_id);
  }

  bool close_and_forget(const std::string& session_id) {
    if (!cleanup_.clear_everything(session_id)) return false;
    if (!manager_.close_and_forget_session(session_id)) return false;
    return application_.destroy_private_session_context(session_id);
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
  PrivateCleanupController cleanup_;
};

}  // namespace goreecloud::browser
