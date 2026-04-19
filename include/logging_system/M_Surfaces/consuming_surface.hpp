#pragma once






/*
------------------------------------------------------------------------------
M_Surfaces/consuming_surface.hpp

Reference Version
-----------------
INFO_SLICE_SYNC_BASELINE_V1

Role in the architecture
------------------------
ConsumingSurface is the consuming-side compile-time façade over the current
finalized INFO slice.

It answers narrow questions such as:
    "How does external code consume the INFO pipeline without touching pipeline
     internals directly?"
    "How can the finalized INFO path be reached through a dedicated consuming
     surface without introducing a monolithic service root?"

Why this file exists in this stage
----------------------------------
The INFO slice now has:
- a concrete pipeline binding
- a runnable/admitted-runtime pipeline runner
- a finalized thin INFO level API entrypoint
- a minimal adapter boundary

At this stage, the consuming surface must evolve from:
- a first runnable façade over `run_single(...)`

into:
- a finalized consuming-side façade that reflects both:
  - the direct helper path
  - the admitted-runtime path

Current minimal scope
---------------------
This file currently provides:
- `ConsumingSurface`
- `log_info(...)`
    direct record-driven runnable helper path
- `admit_and_log_info(...)`
    admitted-runtime INFO path

The intended usage split is:
- `log_info(...)`
    direct helper path
- `admit_and_log_info(...)`
    finalized consuming-side path for the current INFO slice

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- additional dedicated level entries:
  - log_debug
  - log_warn
  - log_error
  - log_fatal
  - log_trace
- raw-content submission helpers once preparation/admission entry is promoted
- readonly inspection/query helpers
- notification/subscription helpers
- consuming-side convenience helpers for CLI/application integration
- eventual alignment with the broader role-separated consuming contract

What should NOT live here
-------------------------
This file must NOT:
- become a system root
- own shared state
- own adapter registries
- own governance/configuration
- collapse dedicated level APIs into generic central convergence
- reimplement pipeline internals

Design rule
-----------
This file is a consuming-side compile-time façade over existing per-level entry
points.
It exposes pipeline-local behavior upward without erasing pipeline boundaries.
------------------------------------------------------------------------------
*/

#include <optional>
#include <string>

#include "logging_system/L_Level_api/log_info.hpp"

namespace logging_system::M_Surfaces {

struct ConsumingSurface final {
    template <typename TModule, typename TRecord, typename TAdapter>
    static auto log_info(
        const TModule& module,
        const TRecord& record,
        TAdapter& adapter,
        const std::optional<std::string>& round_id = std::nullopt) {
        return logging_system::L_Level_api::LogInfo::run_single(
            module,
            record,
            adapter,
            round_id);
    }

    template <typename TModule, typename TRecord, typename TAdapter>
    static auto admit_and_log_info(
        TModule& module,
        const TRecord& record,
        TAdapter& adapter,
        const std::optional<std::string>& round_id = std::nullopt) {
        return logging_system::L_Level_api::LogInfo::admit_and_run(
            module,
            record,
            adapter,
            round_id);
    }
};

}  // namespace logging_system::M_Surfaces

























