#pragma once

#include <optional>
#include <string>
#include <string_view>

#include "goreecloud/browser/application.hpp"
#include "goreecloud/browser/engine.hpp"

namespace goreecloud::browser {

class PrivateCleanupController {
 public:
  explicit PrivateCleanupController(BrowserApplication& application)
      : application_(application) {}

  bool forget_origin(const std::string& session_id, std::string_view origin) {
    auto* context = application_.private_session_context(session_id);
    if (!context) return false;
    return context->clear_origin_data(origin, kAllTemporaryEngineData) &&
           context->clear_authentication_state(origin) &&
           context->clear_permission_state(origin);
  }

  bool clear_cookies(const std::string& session_id) {
    return clear_all(session_id, data_class(EngineDataClass::Cookies));
  }

  bool clear_cache(const std::string& session_id) {
    return clear_all(session_id, data_class(EngineDataClass::HttpCache));
  }

  bool clear_storage(const std::string& session_id) {
    return clear_all(session_id,
                     data_class(EngineDataClass::LocalStorage) |
                     data_class(EngineDataClass::IndexedDb) |
                     data_class(EngineDataClass::ServiceWorkers));
  }

  bool clear_history(const std::string& session_id) {
    return clear_all(session_id, data_class(EngineDataClass::History));
  }

  bool clear_authentication(const std::string& session_id) {
    auto* context = application_.private_session_context(session_id);
    return context && context->clear_authentication_state(std::nullopt);
  }

  bool clear_permissions(const std::string& session_id) {
    auto* context = application_.private_session_context(session_id);
    return context && context->clear_permission_state(std::nullopt);
  }

  bool clear_everything(const std::string& session_id) {
    auto* context = application_.private_session_context(session_id);
    if (!context) return false;
    return context->clear_all_data(kAllTemporaryEngineData) &&
           context->clear_authentication_state(std::nullopt) &&
           context->clear_permission_state(std::nullopt);
  }

 private:
  bool clear_all(const std::string& session_id, EngineDataClasses classes) {
    auto* context = application_.private_session_context(session_id);
    return context && context->clear_all_data(classes);
  }

  BrowserApplication& application_;
};

}  // namespace goreecloud::browser
