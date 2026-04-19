#pragma once

/*
------------------------------------------------------------------------------
M_Surfaces/consuming_surface.hpp

Role in the architecture
------------------------
ConsumingSurface is the first user-facing compile-time consuming surface over
the current runnable INFO slice.

It answers narrow questions such as:
    "How does external code consume the logging subsystem without touching
     pipeline internals directly?"
    "How can the INFO pipeline be reached through a dedicated consuming-side
     surface without introducing a monolithic service root?"

Why this file exists in this stage
----------------------------------
The INFO slice now has:
- a concrete pipeline binding
- a runnable pipeline runner
- a thin INFO level API entrypoint
- a minimal adapter boundary

The next dependency-first step is to provide a small consuming-side surface that
exposes that slice as a user-facing entry, while still preserving:
- per-pipeline specialization
- no shared generic `service.log(level=...)`
- no runtime vtable-centered surface

Current minimal scope
---------------------
This file currently provides:
- `ConsumingSurface`
- a single record-driven INFO entry:
    `log_info(module, record, adapter, round_id)`

This is intentionally minimal.
It closes the first consuming-facing path over the runnable INFO slice.

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
- notification subscription hooks
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
};

}  // namespace logging_system::M_Surfaces