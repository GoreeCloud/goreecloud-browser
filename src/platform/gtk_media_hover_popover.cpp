#include "goreecloud/browser/platform/gtk_media_hover_popover.hpp"

#include <array>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include <gtk/gtk.h>

namespace goreecloud::browser::platform {
namespace {

struct PopoverState {
  GtkWidget* popover{nullptr};
  GtkWidget* quick_row{nullptr};
  GtkWidget* status_row{nullptr};
  GtkWidget* summary{nullptr};
  GtkWidget* more_box{nullptr};
  GtkMediaHoverActionHandler handler;
  std::unordered_map<GtkWidget*, MediaAction> actions;
};

std::unordered_map<GtkWidget*, std::unique_ptr<PopoverState>>& states() {
  static std::unordered_map<GtkWidget*, std::unique_ptr<PopoverState>> map;
  return map;
}

const char* action_label(MediaAction action) {
  switch (action) {
    case MediaAction::preview: return "Preview";
    case MediaAction::search: return "Search";
    case MediaAction::save: return "Save";
    case MediaAction::more: return "More";
    case MediaAction::open_media: return "Open Media";
    case MediaAction::open_media_new_tab: return "Open Media in New Tab";
    case MediaAction::copy_media: return "Copy Media";
    case MediaAction::copy_media_url: return "Copy Media Address";
    case MediaAction::download_media: return "Download Media";
    case MediaAction::select_region: return "Select Area";
    case MediaAction::extract_text: return "Extract Text";
    case MediaAction::translate_text: return "Translate Text";
    case MediaAction::ask_goreecloud_ai: return "Ask GoreeCloud AI";
    case MediaAction::media_details: return "Media Details";
    case MediaAction::security_details: return "Security Details";
    case MediaAction::open_link: return "Open Link";
    case MediaAction::open_link_new_tab: return "Open Link in New Tab";
    case MediaAction::copy_link: return "Copy Link";
    case MediaAction::save_link: return "Save Link";
    case MediaAction::inspect_media: return "Inspect Media";
  }
  return "Media Action";
}

void on_action_clicked(GtkButton* button, gpointer data) {
  auto* state = static_cast<PopoverState*>(data);
  const auto it = state->actions.find(GTK_WIDGET(button));
  if (it == state->actions.end()) return;
  if (state->handler) state->handler(it->second);
}

GtkWidget* make_action_button(PopoverState& state,
                              MediaAction action,
                              const char* label,
                              bool enabled) {
  auto* button = gtk_button_new_with_label(label);
  gtk_widget_set_size_request(button, 44, 44);
  gtk_widget_set_sensitive(button, enabled);
  gtk_widget_set_tooltip_text(button, label);
  gtk_style_context_add_class(gtk_widget_get_style_context(button), "gc-media-hover-action");
  if (auto* accessible = gtk_widget_get_accessible(button)) {
    atk_object_set_name(accessible, label);
  }
  state.actions.emplace(button, action);
  g_signal_connect(button, "clicked", G_CALLBACK(on_action_clicked), &state);
  return button;
}

void add_status_chip(GtkWidget* row, const MediaHoverStatusChip& chip) {
  if (!chip.visible || chip.label.empty()) return;
  auto* label = gtk_label_new(chip.label.c_str());
  gtk_style_context_add_class(gtk_widget_get_style_context(label), "gc-media-hover-chip");
  gtk_box_pack_start(GTK_BOX(row), label, FALSE, FALSE, 0);
}

void install_css(GtkWidget* parent) {
  static bool installed = false;
  if (installed) return;
  auto* screen = gtk_widget_get_screen(parent);
  if (!screen) return;

  auto* provider = gtk_css_provider_new();
  static constexpr const char* kCss = R"CSS(
    .gc-media-hover-popover contents {
      border-radius: 16px;
      padding: 8px;
      background-color: @theme_base_color;
      border: 1px solid alpha(@theme_fg_color, 0.16);
      box-shadow: 0 8px 28px alpha(#000000, 0.24);
    }
    .gc-media-hover-action {
      min-width: 44px;
      min-height: 44px;
      border-radius: 14px;
      padding: 6px 12px;
    }
    .gc-media-hover-chip {
      border-radius: 12px;
      padding: 4px 8px;
      background-color: alpha(@theme_fg_color, 0.08);
    }
    .gc-media-hover-summary {
      padding: 2px 4px 6px 4px;
    }
  )CSS";
  gtk_css_provider_load_from_data(provider, kCss, -1, nullptr);
  gtk_style_context_add_provider_for_screen(
      screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  g_object_unref(provider);
  installed = true;
}

PopoverState& ensure_state(GtkWidget* parent) {
  auto& map = states();
  auto found = map.find(parent);
  if (found != map.end()) return *found->second;

  auto state = std::make_unique<PopoverState>();
  state->popover = gtk_popover_new(parent);
  gtk_style_context_add_class(gtk_widget_get_style_context(state->popover),
                              "gc-media-hover-popover");
  gtk_popover_set_modal(GTK_POPOVER(state->popover), FALSE);
  gtk_popover_set_position(GTK_POPOVER(state->popover), GTK_POS_TOP);

  auto* root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
  gtk_container_add(GTK_CONTAINER(state->popover), root);

  state->summary = gtk_label_new(nullptr);
  gtk_label_set_xalign(GTK_LABEL(state->summary), 0.0F);
  gtk_style_context_add_class(gtk_widget_get_style_context(state->summary),
                              "gc-media-hover-summary");
  gtk_box_pack_start(GTK_BOX(root), state->summary, FALSE, FALSE, 0);

  state->quick_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  gtk_box_pack_start(GTK_BOX(root), state->quick_row, FALSE, FALSE, 0);

  state->status_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  gtk_box_pack_start(GTK_BOX(root), state->status_row, FALSE, FALSE, 0);

  state->more_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
  gtk_box_pack_start(GTK_BOX(root), state->more_box, FALSE, FALSE, 0);

  install_css(parent);
  auto [it, _] = map.emplace(parent, std::move(state));
  return *it->second;
}

void clear_children(GtkWidget* container) {
  GList* children = gtk_container_get_children(GTK_CONTAINER(container));
  for (GList* node = children; node; node = node->next) {
    gtk_widget_destroy(GTK_WIDGET(node->data));
  }
  g_list_free(children);
}

}  // namespace

void show_gtk_media_hover_popover(void* parent_widget,
                                  const MediaHoverViewModel& model,
                                  int viewport_x,
                                  int viewport_y,
                                  GtkMediaHoverActionHandler handler) {
  auto* parent = static_cast<GtkWidget*>(parent_widget);
  if (!parent || !model.visible) return;

  auto& state = ensure_state(parent);
  state.handler = std::move(handler);
  state.actions.clear();
  clear_children(state.quick_row);
  clear_children(state.status_row);
  clear_children(state.more_box);

  gtk_label_set_text(GTK_LABEL(state.summary), model.media_summary.c_str());

  for (const auto& quick : model.quick_actions) {
    auto* button = make_action_button(state,
                                      quick.action,
                                      quick.label.c_str(),
                                      quick.enabled);
    gtk_box_pack_start(GTK_BOX(state.quick_row), button, FALSE, FALSE, 0);
  }

  add_status_chip(state.status_row, model.destination);
  add_status_chip(state.status_row, model.privacy);
  add_status_chip(state.status_row, model.security);

  if (model.expanded) {
    for (const auto action : model.more_actions) {
      auto* button = make_action_button(state, action, action_label(action), true);
      gtk_box_pack_start(GTK_BOX(state.more_box), button, FALSE, FALSE, 0);
    }
    gtk_widget_show(state.more_box);
  } else {
    gtk_widget_hide(state.more_box);
  }

  GdkRectangle rect{};
  rect.x = viewport_x;
  rect.y = viewport_y;
  rect.width = 1;
  rect.height = 1;
  gtk_popover_set_pointing_to(GTK_POPOVER(state.popover), &rect);

  gtk_widget_show_all(state.popover);
  if (!model.expanded) gtk_widget_hide(state.more_box);
}

void hide_gtk_media_hover_popover(void* parent_widget) {
  auto* parent = static_cast<GtkWidget*>(parent_widget);
  if (!parent) return;
  auto found = states().find(parent);
  if (found == states().end()) return;
  gtk_widget_hide(found->second->popover);
  found->second->handler = {};
  found->second->actions.clear();
}

}  // namespace goreecloud::browser::platform
