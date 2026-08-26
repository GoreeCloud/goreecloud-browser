#pragma once

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "goreecloud/browser/session_recovery.hpp"
#include "goreecloud/browser/session_recovery_protection.hpp"

namespace goreecloud::browser {

struct FileSessionRecoveryStoreOptions {
  std::size_t max_checkpoints{5};
  bool fsync_on_write{false};
};

class FileSessionRecoveryStore final : public SessionRecoveryStore {
 public:
  static constexpr std::string_view kProtectionPurpose = "browser.session-recovery.v1";

  FileSessionRecoveryStore(std::filesystem::path directory,
                           SessionRecoveryProtectionBoundary protection,
                           FileSessionRecoveryStoreOptions options = {})
      : directory_(std::move(directory)),
        protection_(std::move(protection)),
        options_(options) {}

  bool write(const SessionCheckpoint& checkpoint) override {
    if (checkpoint.checkpoint_id.empty() || !protection_.persistence_allowed()) return false;
    std::error_code error;
    std::filesystem::create_directories(directory_, error);
    if (error) return false;

    const auto payload = serialize(checkpoint);
    std::string envelope;
    if (protection_.protected_at_rest_available()) {
      const auto protected_payload = protection_.protect(kProtectionPurpose, payload);
      if (!protected_payload) return false;
      envelope = serialize_protected(*protected_payload);
    } else {
      envelope = serialize_development_plaintext(payload);
    }

    const auto final_path = checkpoint_path(checkpoint.checkpoint_id);
    auto temp_path = final_path;
    temp_path += ".tmp";

    {
      std::ofstream out(temp_path, std::ios::binary | std::ios::trunc);
      if (!out) return false;
      out.write(envelope.data(), static_cast<std::streamsize>(envelope.size()));
      out.flush();
      if (!out) return false;
    }

    std::filesystem::rename(temp_path, final_path, error);
    if (error) {
      std::filesystem::remove(final_path, error);
      error.clear();
      std::filesystem::rename(temp_path, final_path, error);
      if (error) {
        std::filesystem::remove(temp_path, error);
        return false;
      }
    }

    rotate();
    return true;
  }

  [[nodiscard]] std::vector<SessionCheckpoint> read_recent(std::size_t limit) const override {
    std::vector<SessionCheckpoint> checkpoints;
    for (const auto& path : checkpoint_files()) {
      const auto parsed = read_one(path);
      if (parsed) checkpoints.push_back(*parsed);
    }
    std::sort(checkpoints.begin(), checkpoints.end(), [](const auto& a, const auto& b) {
      if (a.created_unix_ms != b.created_unix_ms) return a.created_unix_ms > b.created_unix_ms;
      return a.checkpoint_id > b.checkpoint_id;
    });
    if (checkpoints.size() > limit) checkpoints.resize(limit);
    return checkpoints;
  }

  bool erase(const std::string& checkpoint_id) override {
    std::error_code error;
    return std::filesystem::remove(checkpoint_path(checkpoint_id), error) && !error;
  }

 private:
  static std::string escape(std::string_view value) {
    std::ostringstream out;
    for (const unsigned char ch : value) {
      if (ch == '\\' || ch == '\t' || ch == '\n' || ch == '\r') {
        out << '\\' << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(ch)
            << std::dec;
      } else {
        out << static_cast<char>(ch);
      }
    }
    return out.str();
  }

  static std::optional<std::string> unescape(std::string_view value) {
    std::string out;
    for (std::size_t i = 0; i < value.size(); ++i) {
      if (value[i] != '\\') {
        out.push_back(value[i]);
        continue;
      }
      if (i + 2 >= value.size()) return std::nullopt;
      unsigned int byte = 0;
      std::istringstream input(std::string{value.substr(i + 1, 2)});
      input >> std::hex >> byte;
      if (!input) return std::nullopt;
      out.push_back(static_cast<char>(byte));
      i += 2;
    }
    return out;
  }

  static std::uint64_t fnv1a64(std::string_view bytes) {
    std::uint64_t hash = 14695981039346656037ULL;
    for (const unsigned char byte : bytes) {
      hash ^= byte;
      hash *= 1099511628211ULL;
    }
    return hash;
  }

  static std::string hex64(std::uint64_t value) {
    std::ostringstream out;
    out << std::hex << std::setw(16) << std::setfill('0') << value;
    return out.str();
  }

  static std::optional<std::uint64_t> parse_hex64(std::string_view text) {
    std::uint64_t value = 0;
    std::istringstream in(std::string{text});
    in >> std::hex >> value;
    if (!in || !in.eof()) return std::nullopt;
    return value;
  }

  static std::string hex_encode(std::string_view bytes) {
    static constexpr char digits[] = "0123456789abcdef";
    std::string out;
    out.reserve(bytes.size() * 2);
    for (const unsigned char byte : bytes) {
      out.push_back(digits[(byte >> 4) & 0x0f]);
      out.push_back(digits[byte & 0x0f]);
    }
    return out;
  }

  static std::optional<std::string> hex_decode(std::string_view text) {
    if (text.size() % 2 != 0) return std::nullopt;
    auto nibble = [](char ch) -> int {
      if (ch >= '0' && ch <= '9') return ch - '0';
      if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
      if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
      return -1;
    };
    std::string out;
    out.reserve(text.size() / 2);
    for (std::size_t i = 0; i < text.size(); i += 2) {
      const auto high = nibble(text[i]);
      const auto low = nibble(text[i + 1]);
      if (high < 0 || low < 0) return std::nullopt;
      out.push_back(static_cast<char>((high << 4) | low));
    }
    return out;
  }

  static std::string serialize_development_plaintext(std::string_view payload) {
    std::ostringstream out;
    out << "GCRS1\n" << hex64(fnv1a64(payload)) << "\n" << payload;
    return out.str();
  }

  static std::string serialize_protected(const ProtectedRecoveryPayload& payload) {
    std::ostringstream out;
    out << "GCRS2\n"
        << hex_encode(payload.algorithm) << '\n'
        << hex_encode(payload.key_id) << '\n'
        << hex_encode(payload.nonce) << '\n'
        << hex_encode(payload.authentication_tag) << '\n'
        << hex_encode(payload.ciphertext) << '\n';
    return out.str();
  }

  static std::string serialize(const SessionCheckpoint& checkpoint) {
    std::ostringstream out;
    out << "C\t" << escape(checkpoint.checkpoint_id) << '\t' << checkpoint.created_unix_ms << '\t'
        << static_cast<int>(checkpoint.exit_state) << '\n';
    for (const auto& window : checkpoint.windows) {
      out << "W\t" << escape(window.window_id) << '\t' << static_cast<int>(window.privacy_mode) << '\t'
          << escape(window.active_tab_id.value_or("")) << '\n';
      for (const auto& tab : window.tabs) {
        out << "T\t" << escape(tab.tab_id) << '\t' << escape(tab.url) << '\t' << escape(tab.title) << '\t'
            << escape(tab.workspace_id) << '\t' << escape(tab.group_id.value_or("")) << '\t'
            << escape(tab.split_id.value_or("")) << '\t' << (tab.pinned ? 1 : 0) << '\t'
            << (tab.active ? 1 : 0) << '\t' << tab.last_active_unix_ms << '\n';
      }
      out << "E\n";
    }
    return out.str();
  }

  static std::vector<std::string_view> split_tabs(std::string_view line) {
    std::vector<std::string_view> fields;
    std::size_t start = 0;
    while (start <= line.size()) {
      const auto pos = line.find('\t', start);
      fields.push_back(line.substr(start, pos == std::string_view::npos ? line.size() - start : pos - start));
      if (pos == std::string_view::npos) break;
      start = pos + 1;
    }
    return fields;
  }

  static std::optional<SessionCheckpoint> parse_payload(std::string_view payload) {
    std::istringstream input(std::string{payload});
    std::string line;
    SessionCheckpoint checkpoint;
    RecoverableWindow* current_window = nullptr;
    bool saw_checkpoint = false;

    while (std::getline(input, line)) {
      const auto fields = split_tabs(line);
      if (fields.empty()) continue;
      if (fields[0] == "C") {
        if (fields.size() != 4 || saw_checkpoint) return std::nullopt;
        const auto id = unescape(fields[1]);
        if (!id) return std::nullopt;
        checkpoint.checkpoint_id = *id;
        try {
          checkpoint.created_unix_ms = std::stoull(std::string{fields[2]});
          const auto exit = std::stoi(std::string{fields[3]});
          if (exit < 0 || exit > 2) return std::nullopt;
          checkpoint.exit_state = static_cast<SessionExitState>(exit);
        } catch (...) {
          return std::nullopt;
        }
        saw_checkpoint = true;
      } else if (fields[0] == "W") {
        if (!saw_checkpoint || fields.size() != 4) return std::nullopt;
        const auto id = unescape(fields[1]);
        const auto active = unescape(fields[3]);
        if (!id || !active) return std::nullopt;
        int privacy = 0;
        try { privacy = std::stoi(std::string{fields[2]}); } catch (...) { return std::nullopt; }
        if (privacy < 0 || privacy > 2) return std::nullopt;
        checkpoint.windows.push_back(RecoverableWindow{});
        current_window = &checkpoint.windows.back();
        current_window->window_id = *id;
        current_window->privacy_mode = static_cast<SessionPrivacyMode>(privacy);
        if (!active->empty()) current_window->active_tab_id = *active;
      } else if (fields[0] == "T") {
        if (!current_window || fields.size() != 10) return std::nullopt;
        RecoverableTab tab;
        const auto id = unescape(fields[1]);
        const auto url = unescape(fields[2]);
        const auto title = unescape(fields[3]);
        const auto workspace = unescape(fields[4]);
        const auto group = unescape(fields[5]);
        const auto split = unescape(fields[6]);
        if (!id || !url || !title || !workspace || !group || !split) return std::nullopt;
        tab.tab_id = *id;
        tab.url = *url;
        tab.title = *title;
        tab.workspace_id = *workspace;
        if (!group->empty()) tab.group_id = *group;
        if (!split->empty()) tab.split_id = *split;
        try {
          tab.pinned = std::stoi(std::string{fields[7]}) != 0;
          tab.active = std::stoi(std::string{fields[8]}) != 0;
          tab.last_active_unix_ms = std::stoull(std::string{fields[9]});
        } catch (...) {
          return std::nullopt;
        }
        current_window->tabs.push_back(std::move(tab));
      } else if (fields[0] == "E") {
        current_window = nullptr;
      } else {
        return std::nullopt;
      }
    }
    if (!saw_checkpoint || checkpoint.checkpoint_id.empty()) return std::nullopt;
    return checkpoint;
  }

  [[nodiscard]] std::optional<SessionCheckpoint> read_one(const std::filesystem::path& path) const {
    std::ifstream in(path, std::ios::binary);
    if (!in) return std::nullopt;
    std::string magic;
    if (!std::getline(in, magic)) return std::nullopt;

    if (magic == "GCRS1") {
      if (!protection_.persistence_allowed() || protection_.protected_at_rest_available()) {
        return std::nullopt;
      }
      std::string digest_text;
      if (!std::getline(in, digest_text)) return std::nullopt;
      std::ostringstream payload_stream;
      payload_stream << in.rdbuf();
      const auto payload = payload_stream.str();
      const auto expected = parse_hex64(digest_text);
      if (!expected || *expected != fnv1a64(payload)) return std::nullopt;
      return parse_payload(payload);
    }

    if (magic != "GCRS2" || !protection_.protected_at_rest_available()) return std::nullopt;
    std::string algorithm_hex;
    std::string key_id_hex;
    std::string nonce_hex;
    std::string tag_hex;
    std::string ciphertext_hex;
    if (!std::getline(in, algorithm_hex) || !std::getline(in, key_id_hex) ||
        !std::getline(in, nonce_hex) || !std::getline(in, tag_hex) ||
        !std::getline(in, ciphertext_hex)) {
      return std::nullopt;
    }
    std::string unexpected;
    if (std::getline(in, unexpected) && !unexpected.empty()) return std::nullopt;

    const auto algorithm = hex_decode(algorithm_hex);
    const auto key_id = hex_decode(key_id_hex);
    const auto nonce = hex_decode(nonce_hex);
    const auto tag = hex_decode(tag_hex);
    const auto ciphertext = hex_decode(ciphertext_hex);
    if (!algorithm || !key_id || !nonce || !tag || !ciphertext) return std::nullopt;

    const ProtectedRecoveryPayload protected_payload{
        .algorithm = *algorithm,
        .key_id = *key_id,
        .nonce = *nonce,
        .ciphertext = *ciphertext,
        .authentication_tag = *tag,
    };
    const auto plaintext = protection_.unprotect(kProtectionPurpose, protected_payload);
    if (!plaintext) return std::nullopt;
    return parse_payload(*plaintext);
  }

  [[nodiscard]] std::filesystem::path checkpoint_path(std::string_view checkpoint_id) const {
    return directory_ / (safe_filename(checkpoint_id) + ".gcrs");
  }

  static std::string safe_filename(std::string_view value) {
    std::string out;
    out.reserve(value.size());
    for (const unsigned char ch : value) {
      const bool safe = (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
                        (ch >= '0' && ch <= '9') || ch == '-' || ch == '_';
      out.push_back(safe ? static_cast<char>(ch) : '_');
    }
    return out.empty() ? "checkpoint" : out;
  }

  [[nodiscard]] std::vector<std::filesystem::path> checkpoint_files() const {
    std::vector<std::filesystem::path> files;
    std::error_code error;
    if (!std::filesystem::exists(directory_, error) || error) return files;
    for (const auto& entry : std::filesystem::directory_iterator(directory_, error)) {
      if (error) break;
      if (entry.is_regular_file() && entry.path().extension() == ".gcrs") files.push_back(entry.path());
    }
    return files;
  }

  void rotate() const {
    auto checkpoints = read_recent(static_cast<std::size_t>(-1));
    if (checkpoints.size() <= options_.max_checkpoints) return;
    for (std::size_t i = options_.max_checkpoints; i < checkpoints.size(); ++i) {
      std::error_code error;
      std::filesystem::remove(checkpoint_path(checkpoints[i].checkpoint_id), error);
    }
  }

  std::filesystem::path directory_;
  SessionRecoveryProtectionBoundary protection_;
  FileSessionRecoveryStoreOptions options_;
};

}  // namespace goreecloud::browser
