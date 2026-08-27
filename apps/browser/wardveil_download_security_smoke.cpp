#include <cassert>
#include <cstdint>
#include <string>

#include "goreecloud/browser/wardveil_download_security.hpp"

namespace {

using namespace goreecloud::browser;

WardveilDownloadScanResponse response_for(const WardveilDownloadScanRequest& request,
                                           WardveilScanResult result,
                                           std::int64_t observed_at,
                                           std::int64_t valid_until) {
  return {
      true,
      WardveilDownloadScanRecord{
          .contract_version = std::string{kWardveilDownloadContractVersion},
          .record_type = "scan_finding",
          .authoritative_producer = true,
          .producer_id = "wardveil-scan",
          .resource_type = std::string{kWardveilDownloadResourceType},
          .resource_id = request.resource_id,
          .resource_digest_sha256 = request.resource_digest_sha256,
          .result = result,
          .observed_at_unix_seconds = observed_at,
          .valid_until_unix_seconds = valid_until,
          .evidence_refs = {"evidence:test"},
      },
      "ok",
  };
}

}  // namespace

int main() {
  using namespace goreecloud::browser;
  constexpr std::int64_t now = 2'000'000;
  const WardveilDownloadScanRequest request{
      .resource_id = "browser:download:download-42",
      .resource_digest_sha256 = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
      .size_bytes = 42,
      .private_session = false,
  };

  const auto clean = evaluate_wardveil_download_scan(
      request, response_for(request, WardveilScanResult::clean, now - 10, now + 60), now);
  assert(clean.disposition == DownloadSecurityDisposition::allow);
  assert(clean.can_release && clean.can_open && !clean.quarantine_required);

  const auto expired_clean = evaluate_wardveil_download_scan(
      request, response_for(request, WardveilScanResult::clean, now - 60, now - 1), now);
  assert(expired_clean.disposition == DownloadSecurityDisposition::block_unverified);
  assert(!expired_clean.can_release);

  auto no_evidence_response = response_for(request, WardveilScanResult::clean, now - 10, now + 60);
  no_evidence_response.record->evidence_refs.clear();
  const auto no_evidence = evaluate_wardveil_download_scan(request, no_evidence_response, now);
  assert(no_evidence.disposition == DownloadSecurityDisposition::block_unverified);

  const auto malicious = evaluate_wardveil_download_scan(
      request, response_for(request, WardveilScanResult::malicious, now - 10, now + 60), now);
  assert(malicious.disposition == DownloadSecurityDisposition::block_quarantine);
  assert(!malicious.can_release && malicious.quarantine_required);
  const auto quarantine = malicious.quarantine_handoff();
  assert(quarantine && quarantine->requires_explicit_executor_authority);
  assert(!quarantine->destructive_action);

  const auto expired_malicious = evaluate_wardveil_download_scan(
      request, response_for(request, WardveilScanResult::malicious, now - 60, now - 1), now);
  assert(expired_malicious.disposition == DownloadSecurityDisposition::block_quarantine);
  assert(expired_malicious.quarantine_required);

  const auto suspicious = evaluate_wardveil_download_scan(
      request, response_for(request, WardveilScanResult::suspicious, now - 10, now + 60), now);
  assert(suspicious.disposition == DownloadSecurityDisposition::hold_review);
  assert(!suspicious.can_release && !suspicious.quarantine_required);

  const auto unknown = evaluate_wardveil_download_scan(
      request, response_for(request, WardveilScanResult::unknown, now - 10, now + 60), now);
  assert(unknown.disposition == DownloadSecurityDisposition::block_unverified);

  const auto unsupported = evaluate_wardveil_download_scan(
      request, response_for(request, WardveilScanResult::unsupported, now - 10, now + 60), now);
  assert(unsupported.disposition == DownloadSecurityDisposition::block_unverified);

  auto digest_mismatch_response = response_for(request, WardveilScanResult::clean, now - 10, now + 60);
  digest_mismatch_response.record->resource_digest_sha256 =
      "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
  const auto digest_mismatch = evaluate_wardveil_download_scan(request, digest_mismatch_response, now);
  assert(digest_mismatch.disposition == DownloadSecurityDisposition::block_unverified);

  auto scope_mismatch_response = response_for(request, WardveilScanResult::clean, now - 10, now + 60);
  scope_mismatch_response.record->resource_id = "browser:download:download-other";
  const auto scope_mismatch = evaluate_wardveil_download_scan(request, scope_mismatch_response, now);
  assert(scope_mismatch.disposition == DownloadSecurityDisposition::block_unverified);

  auto non_authoritative_response = response_for(request, WardveilScanResult::clean, now - 10, now + 60);
  non_authoritative_response.record->authoritative_producer = false;
  const auto non_authoritative = evaluate_wardveil_download_scan(request, non_authoritative_response, now);
  assert(non_authoritative.disposition == DownloadSecurityDisposition::block_unverified);

  const auto future_dated = evaluate_wardveil_download_scan(
      request, response_for(request, WardveilScanResult::clean, now + 1, now + 60), now);
  assert(future_dated.disposition == DownloadSecurityDisposition::block_unverified);

  const WardveilDownloadScanResponse unavailable{false, std::nullopt, "offline"};
  const auto unavailable_decision = evaluate_wardveil_download_scan(request, unavailable, now);
  assert(unavailable_decision.disposition == DownloadSecurityDisposition::block_unverified);

  return 0;
}
