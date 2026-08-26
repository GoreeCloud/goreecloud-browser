#pragma once

#include <string>
#include <vector>

namespace goreecloud::browser {

struct BrowserLaunchRequest {
  bool private_window{false};
  bool isolated_private_window{false};
  std::vector<std::string> urls;
};

inline BrowserLaunchRequest parse_browser_launch_request(int argc, char** argv) {
  BrowserLaunchRequest request;
  for (int index = 1; index < argc; ++index) {
    const std::string arg = argv[index] ? argv[index] : "";
    if (arg == "--private") {
      request.private_window = true;
      continue;
    }
    if (arg == "--isolated-private") {
      request.private_window = true;
      request.isolated_private_window = true;
      continue;
    }
    if (arg == "--new-window") continue;
    if (arg.rfind("--", 0) == 0) continue;
    if (!arg.empty()) request.urls.push_back(arg);
  }
  return request;
}

}  // namespace goreecloud::browser
