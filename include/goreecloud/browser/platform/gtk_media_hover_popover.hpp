#pragma once

#include <functional>

#include "goreecloud/browser/media_hover_ui.hpp"

namespace goreecloud::browser::platform {

using GtkMediaHoverActionHandler = std::function<void(MediaAction)>;

// The GTK host keeps toolkit details outside the Browser core. parent_widget is
// an opaque GtkWidget* supplied by GtkLinuxGlazeWindowHost.
void show_gtk_media_hover_popover(void* parent_widget,
                                  const MediaHoverViewModel& model,
                                  int viewport_x,
                                  int viewport_y,
                                  GtkMediaHoverActionHandler handler);

void hide_gtk_media_hover_popover(void* parent_widget);

}  // namespace goreecloud::browser::platform
