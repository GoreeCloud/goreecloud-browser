#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <optional>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace goreecloud::browser {

inline constexpr std::string_view kWardveilDownloadContractVersion = "0.1.0";
inline constexpr std::string_view kWardveilDownloadResourceType = "browser_download";

enum class WardveilScanResult { clean, suspicious, malicious, unknown, unsupported };
enum class DownloadSecurityDisposition { allow, hold_review, block_quarantine, block_unverified };

struct WardveilDownloadScanRecord {
  std::string contract_version;
  std::string record_type;
  bool authoritative_producer{false};
  std::string producer_id;
  std::string resource_type;
  std::string resource_id;
  std::string resource_digest_sha256;
  WardveilScanResult result{WardveilScanResult::unknown};
  std::int64_t observed_at_unix_seconds{0};
  std::int64_t valid_until_unix_seconds{0};
  std::vector<std::string> evidence_refs;
};

struct WardveilDownloadScanRequest {
  std::string resource_id;
  std::string resource_digest_sha256;
  std::uint64_t size_bytes{0};
  bool private_session{false};
};

struct WardveilDownloadScanResponse {
  bool transport_succeeded{false};
  std::optional<WardveilDownloadScanRecord> record;
  std::string message;
};

struct DownloadQuarantineHandoff {
  std::string resource_id;
  std::string resource_digest_sha256;
  std::vector<std::string> evidence_refs;
  bool requires_explicit_executor_authority{true};
  bool destructive_action{false};
};

struct DownloadSecurityDecision {
  DownloadSecurityDisposition disposition{DownloadSecurityDisposition::block_unverified};
  bool can_release{false};
  bool can_open{false};
  bool quarantine_required{false};
  std::vector<std::string> reason_codes;
  std::vector<std::string> evidence_refs;
  std::string resource_id;
  std::string resource_digest_sha256;

  [[nodiscard]] std::optional<DownloadQuarantineHandoff> quarantine_handoff() const {
    if (!quarantine_required) return std::nullopt;
    return DownloadQuarantineHandoff{
        .resource_id = resource_id,
        .resource_digest_sha256 = resource_digest_sha256,
        .evidence_refs = evidence_refs,
        .requires_explicit_executor_authority = true,
        .destructive_action = false,
    };
  }
};

class WardveilDownloadScanner {
 public:
  virtual ~WardveilDownloadScanner() = default;
  virtual WardveilDownloadScanResponse scan(
      const WardveilDownloadScanRequest& request,
      const std::filesystem::path& staged_file) = 0;
};

class UnavailableWardveilDownloadScanner final : public WardveilDownloadScanner {
 public:
  WardveilDownloadScanResponse scan(const WardveilDownloadScanRequest&,
                                    const std::filesystem::path&) override {
    return {false, std::nullopt, "Wardveil Scan transport is unavailable."};
  }
};

namespace detail {

class Sha256 final {
 public:
  Sha256() { reset(); }

  void update(std::span<const std::byte> bytes) {
    for (const auto value : bytes) {
      data_[data_length_++] = static_cast<std::uint8_t>(value);
      if (data_length_ == 64) {
        transform();
        bit_length_ += 512;
        data_length_ = 0;
      }
    }
  }

  [[nodiscard]] std::string final_hex() {
    std::size_t index = data_length_;
    data_[index++] = 0x80;
    if (index > 56) {
      while (index < 64) data_[index++] = 0;
      transform();
      index = 0;
    }
    while (index < 56) data_[index++] = 0;

    bit_length_ += static_cast<std::uint64_t>(data_length_) * 8;
    for (int shift = 56; shift >= 0; shift -= 8) {
      data_[index++] = static_cast<std::uint8_t>((bit_length_ >> shift) & 0xffU);
    }
    transform();

    std::ostringstream out;
    out << std::hex << std::setfill('0');
    for (const auto value : state_) out << std::setw(8) << value;
    return out.str();
  }

 private:
  static constexpr std::array<std::uint32_t, 64> kRoundConstants{
      0x428a2f98U,0x71374491U,0xb5c0fbcfU,0xe9b5dba5U,0x3956c25bU,0x59f111f1U,0x923f82a4U,0xab1c5ed5U,
      0xd807aa98U,0x12835b01U,0x243185beU,0x550c7dc3U,0x72be5d74U,0x80deb1feU,0x9bdc06a7U,0xc19bf174U,
      0xe49b69c1U,0xefbe4786U,0x0fc19dc6U,0x240ca1ccU,0x2de92c6fU,0x4a7484aaU,0x5cb0a9dcU,0x76f988daU,
      0x983e5152U,0xa831c66dU,0xb00327c8U,0xbf597fc7U,0xc6e00bf3U,0xd5a79147U,0x06ca6351U,0x14292967U,
      0x27b70a85U,0x2e1b2138U,0x4d2c6dfcU,0x53380d13U,0x650a7354U,0x766a0abbU,0x81c2c92eU,0x92722c85U,
      0xa2bfe8a1U,0xa81a664bU,0xc24b8b70U,0xc76c51a3U,0xd192e819U,0xd6990624U,0xf40e3585U,0x106aa070U,
      0x19a4c116U,0x1e376c08U,0x2748774cU,0x34b0bcb5U,0x391c0cb3U,0x4ed8aa4aU,0x5b9cca4fU,0x682e6ff3U,
      0x748f82eeU,0x78a5636fU,0x84c87814U,0x8cc70208U,0x90befffaU,0xa4506cebU,0xbef9a3f7U,0xc67178f2U};

  static constexpr std::uint32_t rotate_right(std::uint32_t value, std::uint32_t count) {
    return (value >> count) | (value << (32U - count));
  }

  void reset() {
    data_length_ = 0;
    bit_length_ = 0;
    state_ = {0x6a09e667U,0xbb67ae85U,0x3c6ef372U,0xa54ff53aU,
              0x510e527fU,0x9b05688cU,0x1f83d9abU,0x5be0cd19U};
  }

  void transform() {
    std::array<std::uint32_t, 64> schedule{};
    for (std::size_t i = 0; i < 16; ++i) {
      const auto j = i * 4;
      schedule[i] = (static_cast<std::uint32_t>(data_[j]) << 24) |
                    (static_cast<std::uint32_t>(data_[j + 1]) << 16) |
                    (static_cast<std::uint32_t>(data_[j + 2]) << 8) |
                    static_cast<std::uint32_t>(data_[j + 3]);
    }
    for (std::size_t i = 16; i < 64; ++i) {
      const auto s0 = rotate_right(schedule[i - 15], 7) ^ rotate_right(schedule[i - 15], 18) ^ (schedule[i - 15] >> 3);
      const auto s1 = rotate_right(schedule[i - 2], 17) ^ rotate_right(schedule[i - 2], 19) ^ (schedule[i - 2] >> 10);
      schedule[i] = schedule[i - 16] + s0 + schedule[i - 7] + s1;
    }

    auto a = state_[0]; auto b = state_[1]; auto c = state_[2]; auto d = state_[3];
    auto e = state_[4]; auto f = state_[5]; auto g = state_[6]; auto h = state_[7];
    for (std::size_t i = 0; i < 64; ++i) {
      const auto s1 = rotate_right(e, 6) ^ rotate_right(e, 11) ^ rotate_right(e, 25);
      const auto choice = (e & f) ^ ((~e) & g);
      const auto temp1 = h + s1 + choice + kRoundConstants[i] + schedule[i];
      const auto s0 = rotate_right(a, 2) ^ rotate_right(a, 13) ^ rotate_right(a, 22);
      const auto majority = (a & b) ^ (a & c) ^ (b & c);
      const auto temp2 = s0 + majority;
      h = g; g = f; f = e; e = d + temp1;
      d = c; c = b; b = a; a = temp1 + temp2;
    }
    state_[0] += a; state_[1] += b; state_[2] += c; state_[3] += d;
    state_[4] += e; state_[5] += f; state_[6] += g; state_[7] += h;
  }

  std::array<std::uint8_t, 64> data_{};
  std::size_t data_length_{0};
  std::uint64_t bit_length_{0};
  std::array<std::uint32_t, 8> state_{};
};

}  // namespace detail

inline std::optional<std::string> sha256_file(const std::filesystem::path& path) {
  std::ifstream stream(path, std::ios::binary);
  if (!stream) return std::nullopt;
  detail::Sha256 digest;
  std::array<char, 8192> buffer{};
  while (stream) {
    stream.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    const auto count = stream.gcount();
    if (count > 0) {
      const auto chars = std::span<const char>{buffer.data(), static_cast<std::size_t>(count)};
      digest.update(std::as_bytes(chars));
    }
  }
  if (!stream.eof()) return std::nullopt;
  return digest.final_hex();
}

inline std::int64_t wardveil_unix_now_seconds() {
  return std::chrono::duration_cast<std::chrono::seconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}

inline std::string browser_download_resource_id(std::string_view download_id) {
  return "browser:download:" + std::string{download_id};
}

inline DownloadSecurityDecision evaluate_wardveil_download_scan(
    const WardveilDownloadScanRequest& request,
    const WardveilDownloadScanResponse& response,
    std::int64_t now_unix_seconds = wardveil_unix_now_seconds()) {
  DownloadSecurityDecision decision;
  decision.resource_id = request.resource_id;
  decision.resource_digest_sha256 = request.resource_digest_sha256;

  const auto block_unverified = [&](std::string reason) {
    decision.disposition = DownloadSecurityDisposition::block_unverified;
    decision.can_release = false;
    decision.can_open = false;
    decision.quarantine_required = false;
    decision.reason_codes = {std::move(reason)};
    return decision;
  };

  if (!response.transport_succeeded || !response.record) {
    return block_unverified("wardveil_scan_unavailable");
  }
  const auto& record = *response.record;
  if (record.contract_version != kWardveilDownloadContractVersion) return block_unverified("unsupported_scan_contract");
  if (record.record_type != "scan_finding") return block_unverified("invalid_scan_record_type");
  if (!record.authoritative_producer || record.producer_id.empty()) return block_unverified("non_authoritative_scan_record");
  if (record.resource_type != kWardveilDownloadResourceType || record.resource_id != request.resource_id) return block_unverified("scan_scope_mismatch");
  if (record.resource_digest_sha256 != request.resource_digest_sha256) return block_unverified("scan_digest_mismatch");
  if (record.observed_at_unix_seconds <= 0 || record.valid_until_unix_seconds <= record.observed_at_unix_seconds) return block_unverified("invalid_scan_time_window");
  if (record.observed_at_unix_seconds > now_unix_seconds) return block_unverified("future_dated_scan_evidence");

  decision.evidence_refs = record.evidence_refs;
  if (record.result == WardveilScanResult::malicious) {
    decision.disposition = DownloadSecurityDisposition::block_quarantine;
    decision.can_release = false;
    decision.can_open = false;
    decision.quarantine_required = true;
    decision.reason_codes = {"malicious_content_detected"};
    return decision;
  }

  if (record.valid_until_unix_seconds <= now_unix_seconds) {
    return block_unverified("scan_evidence_expired");
  }

  switch (record.result) {
    case WardveilScanResult::clean:
      if (record.evidence_refs.empty()) return block_unverified("clean_evidence_missing");
      decision.disposition = DownloadSecurityDisposition::allow;
      decision.can_release = true;
      decision.can_open = true;
      decision.reason_codes = {"current_authoritative_clean"};
      return decision;
    case WardveilScanResult::suspicious:
      decision.disposition = DownloadSecurityDisposition::hold_review;
      decision.can_release = false;
      decision.can_open = false;
      decision.reason_codes = {"suspicious_content_review_required"};
      return decision;
    case WardveilScanResult::unknown:
      return block_unverified("scan_result_unknown");
    case WardveilScanResult::unsupported:
      return block_unverified("scan_result_unsupported");
    case WardveilScanResult::malicious:
      break;
  }
  return block_unverified("scan_result_invalid");
}

}  // namespace goreecloud::browser
