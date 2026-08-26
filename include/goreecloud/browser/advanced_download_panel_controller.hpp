#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

#include "goreecloud/browser/advanced_download_panel.hpp"

namespace goreecloud::browser {

enum class DownloadPanelAction {
  pause,
  resume,
  cancel,
  cancel_and_discard,
  restart,
  open,
};

struct DownloadPanelActionResult {
  bool accepted{false};
  std::string message;
  std::optional<std::filesystem::path> open_path;
};

class AdvancedDownloadPanelController {
 public:
  explicit AdvancedDownloadPanelController(AdvancedDownloadRuntimeService& runtime)
      : runtime_(runtime) {}

  void set_filter(DownloadPanelFilter filter) noexcept { filter_ = filter; }
  void set_sort(DownloadPanelSort sort) noexcept { sort_ = sort; }

  [[nodiscard]] DownloadPanelFilter filter() const noexcept { return filter_; }
  [[nodiscard]] DownloadPanelSort sort() const noexcept { return sort_; }

  [[nodiscard]] AdvancedDownloadPanelModel model() const {
    return AdvancedDownloadPanelBuilder::build(runtime_, filter_, sort_);
  }

  DownloadPanelActionResult invoke(DownloadPanelAction action,
                                   std::string_view download_id) {
    switch (action) {
      case DownloadPanelAction::pause:
        return runtime_.pause(download_id)
                   ? DownloadPanelActionResult{true, "Download paused.", std::nullopt}
                   : DownloadPanelActionResult{false, "Download cannot be paused in its current state.", std::nullopt};
      case DownloadPanelAction::resume:
        return runtime_.resume(download_id)
                   ? DownloadPanelActionResult{true, "Download resumed.", std::nullopt}
                   : DownloadPanelActionResult{false, "Download cannot be resumed in its current state.", std::nullopt};
      case DownloadPanelAction::cancel:
        return runtime_.cancel(download_id, false)
                   ? DownloadPanelActionResult{true, "Download cancelled.", std::nullopt}
                   : DownloadPanelActionResult{false, "Download cannot be cancelled in its current state.", std::nullopt};
      case DownloadPanelAction::cancel_and_discard:
        return runtime_.cancel(download_id, true)
                   ? DownloadPanelActionResult{true, "Download cancelled and partial data removed.", std::nullopt}
                   : DownloadPanelActionResult{false, "Download cannot be cancelled in its current state.", std::nullopt};
      case DownloadPanelAction::restart:
        return runtime_.restart(download_id)
                   ? DownloadPanelActionResult{true, "Download restarted.", std::nullopt}
                   : DownloadPanelActionResult{false, "Download cannot be restarted in its current state.", std::nullopt};
      case DownloadPanelAction::open: {
        const auto path = runtime_.completed_file_path(download_id);
        if (!path) {
          return {false, "Completed download file is unavailable.", std::nullopt};
        }
        return {true, "Completed download is ready to open.", path};
      }
    }
    return {false, "Unknown download action.", std::nullopt};
  }

 private:
  AdvancedDownloadRuntimeService& runtime_;
  DownloadPanelFilter filter_{DownloadPanelFilter::all};
  DownloadPanelSort sort_{DownloadPanelSort::queue_order};
};

}  // namespace goreecloud::browser
