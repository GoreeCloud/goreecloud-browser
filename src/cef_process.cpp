#include "goreecloud/browser/cef_process.hpp"

#if GOREECLOUD_ENABLE_CEF
#include "goreecloud/browser/cef_media_probe_app.hpp"
#include "include/cef_app.h"
#endif

namespace goreecloud::browser {

int execute_cef_subprocess_if_needed(int argc, char** argv) {
#if GOREECLOUD_ENABLE_CEF
  CefMainArgs main_args(argc, argv);
  CefRefPtr<GoreeCloudCefRenderApp> app = new GoreeCloudCefRenderApp();
  const int exit_code = CefExecuteProcess(main_args, app, nullptr);
  return exit_code;
#else
  (void)argc;
  (void)argv;
  return -1;
#endif
}

}  // namespace goreecloud::browser
