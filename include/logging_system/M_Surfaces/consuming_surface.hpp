#pragma once



#pragma once

/*
------------------------------------------------------------------------------
M_Surfaces/consuming_surface.hpp

Reference Version
-----------------
MULTI_LEVEL_CONSUMING_SURFACE_BASELINE_V1

Role in the architecture
------------------------
ConsumingSurface is the consuming-side compile-time façade over the currently
closed per-level pipeline slices.

It answers narrow questions such as:
    "How does external code consume dedicated log-level pipelines without
     touching pipeline internals directly?"
    "How can finalized per-level paths be reached through one consuming-side
     surface without reintroducing a monolithic shared service root?"

Why this file exists in this stage
----------------------------------
The system now has closed per-level baselines for:
- INFO
- DEBUG
- WARN
- ERROR
- FATAL
- TRACE

Each level now exposes a dedicated thin entrypoint over its own pipeline slice.
At this stage, the consuming surface must expand from:
- an INFO-only consuming façade

into:
- a multi-level consuming façade that reflects all closed level baselines while
  preserving:
  - dedicated per-level entrypoints
  - compile-time composition
  - no central service.log(level=...) convergence

Current minimal scope
---------------------
This file currently provides:
- `ConsumingSurface`
- INFO:
  - `log_info(...)`
  - `admit_and_log_info(...)`
- DEBUG:
  - `log_debug(...)`
  - `admit_and_log_debug(...)`
- WARN:
  - `log_warn(...)`
  - `admit_and_log_warn(...)`
- ERROR:
  - `log_error(...)`
  - `admit_and_log_error(...)`
- FATAL:
  - `log_fatal(...)`
  - `admit_and_log_fatal(...)`
- TRACE:
  - `log_trace(...)`
  - `admit_and_log_trace(...)`

The intended usage split is preserved per level:
- `log_<level>(...)`
    direct record-driven runnable helper path
- `admit_and_log_<level>(...)`
    admitted-runtime path for the finalized slice

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- raw-content submission helpers once preparation/admission entry is promoted
- readonly inspection/query helpers
- notification/subscription helpers
- consuming-side convenience helpers for CLI/application integration
- eventual alignment with broader role-separated consuming contracts

What should NOT live here
-------------------------
This file must NOT:
- become a system root
- own shared state
- own adapter registries
- own governance/configuration
- collapse dedicated level APIs into generic central convergence
- reimplement pipeline internals
- perform runtime level switching through string/enum multiplexing

Design rule
-----------
This file is a consuming-side compile-time façade over existing per-level entry
points.
It exposes pipeline-local behavior upward without erasing pipeline boundaries
or collapsing the architecture back into a shared runtime convergence point.
------------------------------------------------------------------------------
*/

#include <optional>
#include <string>

#include "logging_system/L_Level_api/log_debug.hpp"
#include "logging_system/L_Level_api/log_error.hpp"
#include "logging_system/L_Level_api/log_fatal.hpp"
#include "logging_system/L_Level_api/log_info.hpp"
#include "logging_system/L_Level_api/log_trace.hpp"
#include "logging_system/L_Level_api/log_warn.hpp"

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

    template <typename TModule, typename TRecord, typename TAdapter>
    static auto log_debug(
        const TModule& module,
        const TRecord& record,
        TAdapter& adapter,
        const std::optional<std::string>& round_id = std::nullopt) {
        return logging_system::L_Level_api::LogDebug::run_single(
            module,
            record,
            adapter,
            round_id);
    }

    template <typename TModule, typename TRecord, typename TAdapter>
    static auto admit_and_log_debug(
        TModule& module,
        const TRecord& record,
        TAdapter& adapter,
        const std::optional<std::string>& round_id = std::nullopt) {
        return logging_system::L_Level_api::LogDebug::admit_and_run(
            module,
            record,
            adapter,
            round_id);
    }

    template <typename TModule, typename TRecord, typename TAdapter>
    static auto log_warn(
        const TModule& module,
        const TRecord& record,
        TAdapter& adapter,
        const std::optional<std::string>& round_id = std::nullopt) {
        return logging_system::L_Level_api::LogWarn::run_single(
            module,
            record,
            adapter,
            round_id);
    }

    template <typename TModule, typename TRecord, typename TAdapter>
    static auto admit_and_log_warn(
        TModule& module,
        const TRecord& record,
        TAdapter& adapter,
        const std::optional<std::string>& round_id = std::nullopt) {
        return logging_system::L_Level_api::LogWarn::admit_and_run(
            module,
            record,
            adapter,
            round_id);
    }

    template <typename TModule, typename TRecord, typename TAdapter>
    static auto log_error(
        const TModule& module,
        const TRecord& record,
        TAdapter& adapter,
        const std::optional<std::string>& round_id = std::nullopt) {
        return logging_system::L_Level_api::LogError::run_single(
            module,
            record,
            adapter,
            round_id);
    }

    template <typename TModule, typename TRecord, typename TAdapter>
    static auto admit_and_log_error(
        TModule& module,
        const TRecord& record,
        TAdapter& adapter,
        const std::optional<std::string>& round_id = std::nullopt) {
        return logging_system::L_Level_api::LogError::admit_and_run(
            module,
            record,
            adapter,
            round_id);
    }

    template <typename TModule, typename TRecord, typename TAdapter>
    static auto log_fatal(
        const TModule& module,
        const TRecord& record,
        TAdapter& adapter,
        const std::optional<std::string>& round_id = std::nullopt) {
        return logging_system::L_Level_api::LogFatal::run_single(
            module,
            record,
            adapter,
            round_id);
    }

    template <typename TModule, typename TRecord, typename TAdapter>
    static auto admit_and_log_fatal(
        TModule& module,
        const TRecord& record,
        TAdapter& adapter,
        const std::optional<std::string>& round_id = std::nullopt) {
        return logging_system::L_Level_api::LogFatal::admit_and_run(
            module,
            record,
            adapter,
            round_id);
    }

    template <typename TModule, typename TRecord, typename TAdapter>
    static auto log_trace(
        const TModule& module,
        const TRecord& record,
        TAdapter& adapter,
        const std::optional<std::string>& round_id = std::nullopt) {
        return logging_system::L_Level_api::LogTrace::run_single(
            module,
            record,
            adapter,
            round_id);
    }

    template <typename TModule, typename TRecord, typename TAdapter>
    static auto admit_and_log_trace(
        TModule& module,
        const TRecord& record,
        TAdapter& adapter,
        const std::optional<std::string>& round_id = std::nullopt) {
        return logging_system::L_Level_api::LogTrace::admit_and_run(
            module,
            record,
            adapter,
            round_id);
    }
};

}  // namespace logging_system::M_Surfaces























