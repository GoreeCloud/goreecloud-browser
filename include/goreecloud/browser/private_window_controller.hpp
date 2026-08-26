#pragma once

#include <string>

#include "goreecloud/browser/private_browsing.hpp"
#include "goreecloud/browser/window_controller.hpp"

namespace goreecloud::browser {

class PrivateWindowController {
 public:
  PrivateWindowController(WindowController& window,
                          PrivateBrowsingManager& private_manager,
                          std::string session_id,
                          std::string window_id)
      : window_(window), private_manager_(private_manager), session_id_(std::move(session_id)), window_id_(std::move(window_id)) {
    private_manager_.attach_window(session_id_, window_id_);
  }

  [[nodiscard]] const std::string& session_id() const noexcept { return session_id_; }
  [[nodiscard]] const std::string& window_id() const noexcept { return window_id_; }

  [[nodiscard]] Tab& new_private_tab() { return window_.new_tab(); }

  bool forget_tab(std::string_view tab_id) {
    if (!private_manager_.forget_tab(session_id_, std::string{tab_id})) return false;
    return window_.close_tab(tab_id, true);
  }

  bool close_and_forget_window() {
    return private_manager_.close_and_forget_session(session_id_);
  }

  bool close_and_forget_session() {
    return private_manager_.close_and_forget_session(session_id_);
  }

  [[nodiscard]] std::optional<PrivateSessionDescriptor> inspect() const {
    return private_manager_.inspect(session_id_);
  }

 private:
  WindowController& window_;
  PrivateBrowsingManager& private_manager_;
  std::string session_id_;
  std::string window_id_;
};

}  // namespace goreecloud::browser
