#pragma once

#include <memory>

#include "goreecloud/browser/chromium_adapter.hpp"
#include "goreecloud/browser/chromium_runtime_delegate.hpp"

namespace goreecloud::browser {

[[nodiscard]] std::unique_ptr<ChromiumRuntimeDelegate>
create_chromium_runtime_delegate(const ChromiumAdapterOptions& options);

}  // namespace goreecloud::browser
