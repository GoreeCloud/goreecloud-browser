#include "goreecloud/browser/platform/gtk_linux_glaze_host.hpp"

#include <gtk/gtk.h>

namespace goreecloud::browser::platform {

bool GtkLinuxGlazeWindowHost::copy_text_to_clipboard(std::string_view text) {
  if (text.empty()) return false;
  auto* clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
  if (!clipboard) return false;
  const std::string value{text};
  gtk_clipboard_set_text(clipboard, value.c_str(), static_cast<gint>(value.size()));
  gtk_clipboard_store(clipboard);
  return true;
}

std::optional<MediaSaveDestination> GtkLinuxGlazeWindowHost::choose_media_save_destination(
    MediaKind kind) {
  auto* dialog = gtk_dialog_new_with_buttons(
      "Save Media",
      nullptr,
      GTK_DIALOG_MODAL,
      "Cancel", GTK_RESPONSE_CANCEL,
      "Local Device — Local", 1,
      "GoreeCloud Drive — Synchronized", 2,
      nullptr);

  if (kind == MediaKind::video || kind == MediaKind::embedded_media) {
    gtk_dialog_add_button(GTK_DIALOG(dialog), "GoreeCloud Video — Synchronized", 4);
  } else {
    gtk_dialog_add_button(GTK_DIALOG(dialog), "GoreeCloud Photos — Synchronized", 3);
  }

  if (auto* content = gtk_dialog_get_content_area(GTK_DIALOG(dialog))) {
    auto* label = gtk_label_new(
        "Choose where this media should be stored. Synchronized destinations require their GoreeCloud service adapter and explicit persistence confirmation.");
    gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
    gtk_widget_set_margin_start(label, 16);
    gtk_widget_set_margin_end(label, 16);
    gtk_widget_set_margin_top(label, 12);
    gtk_widget_set_margin_bottom(label, 12);
    gtk_box_pack_start(GTK_BOX(content), label, FALSE, FALSE, 0);
  }

  gtk_widget_show_all(dialog);
  const int response = gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);

  switch (response) {
    case 1: return MediaSaveDestination::local_device;
    case 2: return MediaSaveDestination::goreecloud_drive;
    case 3: return MediaSaveDestination::goreecloud_photos;
    case 4: return MediaSaveDestination::goreecloud_video;
    default: return std::nullopt;
  }
}

bool GtkLinuxGlazeWindowHost::confirm_media_boundary(std::string_view title,
                                                      std::string_view explanation) {
  const std::string title_text{title};
  const std::string body_text{explanation};
  auto* dialog = gtk_message_dialog_new(
      nullptr,
      GTK_DIALOG_MODAL,
      GTK_MESSAGE_QUESTION,
      GTK_BUTTONS_NONE,
      "%s",
      body_text.c_str());
  gtk_window_set_title(GTK_WINDOW(dialog), title_text.c_str());
  gtk_dialog_add_button(GTK_DIALOG(dialog), "Cancel", GTK_RESPONSE_CANCEL);
  gtk_dialog_add_button(GTK_DIALOG(dialog), "Continue", GTK_RESPONSE_ACCEPT);
  const int response = gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
  return response == GTK_RESPONSE_ACCEPT;
}

void GtkLinuxGlazeWindowHost::show_media_action_status(std::string_view message) {
  if (!message.empty()) show_panel(message);
}

}  // namespace goreecloud::browser::platform
