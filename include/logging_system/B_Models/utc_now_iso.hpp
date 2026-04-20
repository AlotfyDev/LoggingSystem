#pragma once

/*
------------------------------------------------------------------------------
B_Models/utc_now_iso.hpp

Reference Version
-----------------
CONSUMING_PIPELINES_CORRECTION_BASELINE_V1

Role in the architecture
------------------------
This file provides UTC time extraction helpers for the logging system models and
preparation components.

It answers narrow questions such as:
    "How does the system obtain the current UTC time in a machine-friendly
     numeric form for ordering, querying, and time arithmetic?"
    "How does the system obtain a human-readable UTC ISO-8601 string when
     presentation or export requires it?"
    "How can timestamp extraction remain isolated from envelope, record, and
     assembler logic?"

Why this file exists in this stage
----------------------------------
The consuming-pipeline correction clarified that:
- envelope timestamps should be numeric in the core model
- numeric timestamps are better suited for:
  - ordering
  - comparison
  - query windows
  - arithmetic
- ISO string timestamps may still be useful for:
  - display
  - export
  - diagnostics

This means the time utility layer should provide:
- a numeric UTC epoch timestamp function
- an ISO UTC string function
- no envelope mutation logic
- no registry/state logic
- no validation logic

Current minimal scope
---------------------
This file currently provides:
- `utc_now_epoch_millis()`
- `utc_now_iso()`

`utc_now_epoch_millis()` is the primary helper for core model timestamps.

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- epoch microseconds helpers
- parsing helpers from ISO to epoch
- formatting helpers from epoch to ISO
- monotonic helper utilities for runtime-local measurements
- clock abstraction hooks for deterministic tests

What should NOT live here
-------------------------
This file must NOT:
- mutate envelopes
- mutate records
- manage slot lifecycle
- perform validation
- assemble content
- become a system clock service

Design rule
-----------
This file provides low-level UTC extraction helpers only.
It exists so higher-level models can use stable, isolated time primitives
without embedding time-extraction logic directly into unrelated components.
------------------------------------------------------------------------------
*/

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

namespace logging_system::B_Models {

[[nodiscard]] inline std::int64_t utc_now_epoch_millis() {
    const auto now = std::chrono::system_clock::now();
    const auto millis =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch());
    return static_cast<std::int64_t>(millis.count());
}

[[nodiscard]] inline std::string utc_now_iso() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    std::tm utc_tm{};
#if defined(_WIN32)
    gmtime_s(&utc_tm, &now_c);
#else
    gmtime_r(&now_c, &utc_tm);
#endif

    std::ostringstream oss{};
    oss << std::put_time(&utc_tm, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

}  // namespace logging_system::B_Models