#pragma once

#include <memory>
#include <utility>

#include "goreecloud/browser/services.hpp"

namespace goreecloud::browser {

struct BrowserServices {
  std::shared_ptr<SearchService> search;
  std::shared_ptr<BookmarkService> bookmarks;
  std::shared_ptr<SyncService> sync;
  std::shared_ptr<VaultService> vault;
  std::shared_ptr<DnsService> dns;
  std::shared_ptr<NetworkService> network;
  std::shared_ptr<PrivacyShieldService> privacy_shield;
  std::shared_ptr<WardveilService> wardveil;
};

class ServiceRegistry {
 public:
  explicit ServiceRegistry(BrowserServices services)
      : services_(std::move(services)) {}

  [[nodiscard]] const BrowserServices& services() const noexcept {
    return services_;
  }

 private:
  BrowserServices services_;
};

}  // namespace goreecloud::browser
