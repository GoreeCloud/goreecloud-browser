#pragma once

namespace goreecloud::browser {

struct PrivateBrowsingPolicy {
  bool privacy_shield_active{true};
  bool durable_operational_logging{false};
  bool retain_clipboard_history{false};
  bool expose_normal_clipboard_history{false};
  bool synchronize_session_state{false};
  bool use_for_behavioral_analytics{false};
  bool preserve_dns_security{true};
  bool preserve_network_security{true};
  bool preserve_certificate_security{true};
};

}  // namespace goreecloud::browser
