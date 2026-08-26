#include "goreecloud/browser/platform/gtk_linux_glaze_host.hpp"

#include <string>

#include <gdk-pixbuf/gdk-pixbuf.h>
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

bool GtkLinuxGlazeWindowHost::show_media_preview(
    const MediaPreviewFrame& frame,
    std::string_view accessible_description) {
  if (frame.encoded_bytes.empty() || frame.width <= 0 || frame.height <= 0) return false;

  GError* error = nullptr;
  auto* loader = gdk_pixbuf_loader_new();
  if (!loader) return false;

  const gboolean wrote = gdk_pixbuf_loader_write(
      loader,
      frame.encoded_bytes.data(),
      static_cast<gsize>(frame.encoded_bytes.size()),
      &error);
  const gboolean closed = wrote ? gdk_pixbuf_loader_close(loader, &error) : FALSE;
  auto* pixbuf = closed ? gdk_pixbuf_loader_get_pixbuf(loader) : nullptr;
  if (!pixbuf) {
    if (error) g_error_free(error);
    g_object_unref(loader);
    return false;
  }

  g_object_ref(pixbuf);
  g_object_unref(loader);

  auto* dialog = gtk_dialog_new_with_buttons(
      "Media Preview",
      nullptr,
      GTK_DIALOG_MODAL,
      "Close", GTK_RESPONSE_CLOSE,
      nullptr);
  gtk_window_set_default_size(GTK_WINDOW(dialog), 720, 560);
  gtk_style_context_add_class(gtk_widget_get_style_context(dialog), "gc-media-preview");

  auto* content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
  auto* image = gtk_image_new_from_pixbuf(pixbuf);
  gtk_widget_set_hexpand(image, TRUE);
  gtk_widget_set_vexpand(image, TRUE);
  gtk_widget_set_halign(image, GTK_ALIGN_CENTER);
  gtk_widget_set_valign(image, GTK_ALIGN_CENTER);
  if (!accessible_description.empty()) {
    if (auto* accessible = gtk_widget_get_accessible(image)) {
      const std::string description{accessible_description};
      atk_object_set_name(accessible, description.c_str());
    }
  }
  gtk_box_pack_start(GTK_BOX(content), image, TRUE, TRUE, 12);

  gtk_widget_show_all(dialog);
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
  g_object_unref(pixbuf);
  if (error) g_error_free(error);
  return true;
}

void GtkLinuxGlazeWindowHost::show_media_action_status(std::string_view message) {
  if (!message.empty()) show_panel(message);
}

}  // namespace goreecloud::browser::platform
