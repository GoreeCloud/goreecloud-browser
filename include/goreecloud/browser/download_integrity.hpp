#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

namespace goreecloud::browser {

struct DownloadIntegrityExpectation {
  std::optional<std::uint64_t> expected_size;
  std::optional<std::string> expected_sha256;
};

struct DownloadIntegrityResult {
  bool verified{false};
  std::string message;
};

class DownloadIntegrityVerifier {
 public:
  virtual ~DownloadIntegrityVerifier() = default;
  virtual DownloadIntegrityResult verify(
      const std::filesystem::path& file,
      const DownloadIntegrityExpectation& expectation) = 0;
};

class SizeOnlyDownloadIntegrityVerifier final : public DownloadIntegrityVerifier {
 public:
  DownloadIntegrityResult verify(
      const std::filesystem::path& file,
      const DownloadIntegrityExpectation& expectation) override {
    if (!expectation.expected_size) {
      return {false, "No integrity size expectation is available."};
    }
    std::error_code error;
    const auto size = std::filesystem::file_size(file, error);
    if (error) return {false, "Downloaded file size could not be read."};
    if (size != *expectation.expected_size) {
      return {false, "Downloaded file size does not match the expected size."};
    }
    if (expectation.expected_sha256) {
      return {false, "SHA-256 verification requires a cryptographic verifier implementation."};
    }
    return {true, "Downloaded file size verified."};
  }
};

}  // namespace goreecloud::browser
