#pragma once

namespace goreecloud::browser {

// Returns an exit code >= 0 when the current process is a CEF subprocess and
// has completed its work. Returns -1 when execution should continue as the
// GoreeCloud Browser process.
int execute_cef_subprocess_if_needed(int argc, char** argv);

}  // namespace goreecloud::browser
