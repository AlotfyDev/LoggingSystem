#pragma once

/*
------------------------------------------------------------------------------
M_Surfaces/consuming_surface.hpp

Reference Version
-----------------
CONSUMING_PIPELINES_CONSISTENCY_FIX_V1

Role in the architecture
------------------------
ConsumingSurface is the consuming-side façade over the dedicated level API
objects.

It answers narrow questions such as:
    "How does external code consume dedicated log-level APIs without touching
     assembler internals directly?"
    "How can the system expose per-level consumer-facing entrypoints while
     preserving thin APIs and avoiding generic shared service.log(level=...)
     convergence?"
    "How can the consuming surface remain a surface object over owned level APIs
     rather than a runtime façade over records and dispatch paths?"

Why this file exists in this stage
----------------------------------
The consuming-pipeline correction clarified that:
- level APIs must no longer be record-driven façades
- each level API is an independent object surface
- each level API owns its specialized assembler by composition
- the consumer should see only content-facing methods
- schema/configuration concerns must remain hidden from the consumer
- the consuming surface should expose the level APIs, not bypass them

This means the consuming surface must evolve from:
- a static record-driven façade over pipeline runners

into:
- an object surface over owned level API objects
- with consumer-facing content-only calls
- no direct record/runtime exposure

Current minimal scope
---------------------
This file currently provides:
- `ConsumingSurface<TLogDebug, TLogInfo, TLogWarn, TLogError, TLogFatal, TLogTrace>`
- owned API objects for:
  - debug
  - info
  - warn
  - error
  - fatal
  - trace
- consumer-facing forwarding methods:
  - `LogDebug(...)`
  - `LogInfo(...)`
  - `LogWarn(...)`
  - `LogError(...)`
  - `LogFatal(...)`
  - `LogTrace(...)`
- a static factory method for controlled surface construction

Each method accepts content only and forwards it to the owned level API object.

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- default aliases over default level API types
- readonly/query-oriented consuming helpers
- event/subscription helpers
- CLI/application-oriented façade aliases
- stronger compile-time constraints over owned API objects

What should NOT live here
-------------------------
This file must NOT:
- become a system root
- own registry state
- own dispatch/runtime logic
- expose schema-level administrative controls to consumers
- collapse dedicated level APIs into a generic shared level switch
- reintroduce record-driven consumer entrypoints

Design rule
-----------
This file defines the consuming-side façade over dedicated level API objects.

The consuming surface owns per-level API objects.
Each level API remains the only consumer-visible entry to its own owned
assembler.
The consuming surface forwards content to those APIs and nothing more.
------------------------------------------------------------------------------
*/

#include <utility>

namespace logging_system::M_Surfaces {

template <
    typename TLogDebug,
    typename TLogInfo,
    typename TLogWarn,
    typename TLogError,
    typename TLogFatal,
    typename TLogTrace>
class ConsumingSurface final {
public:
    using LogDebugApi = TLogDebug;
    using LogInfoApi = TLogInfo;
    using LogWarnApi = TLogWarn;
    using LogErrorApi = TLogError;
    using LogFatalApi = TLogFatal;
    using LogTraceApi = TLogTrace;

    ConsumingSurface() = default;

    ConsumingSurface(
        TLogDebug log_debug_in,
        TLogInfo log_info_in,
        TLogWarn log_warn_in,
        TLogError log_error_in,
        TLogFatal log_fatal_in,
        TLogTrace log_trace_in)
        : log_debug_(std::move(log_debug_in)),
          log_info_(std::move(log_info_in)),
          log_warn_(std::move(log_warn_in)),
          log_error_(std::move(log_error_in)),
          log_fatal_(std::move(log_fatal_in)),
          log_trace_(std::move(log_trace_in)) {}

    [[nodiscard]] static ConsumingSurface Create(
        TLogDebug log_debug,
        TLogInfo log_info,
        TLogWarn log_warn,
        TLogError log_error,
        TLogFatal log_fatal,
        TLogTrace log_trace) {
        return ConsumingSurface{
            std::move(log_debug),
            std::move(log_info),
            std::move(log_warn),
            std::move(log_error),
            std::move(log_fatal),
            std::move(log_trace)
        };
    }

    [[nodiscard]] TLogDebug& debug_api() noexcept {
        return log_debug_;
    }

    [[nodiscard]] const TLogDebug& debug_api() const noexcept {
        return log_debug_;
    }

    [[nodiscard]] TLogInfo& info_api() noexcept {
        return log_info_;
    }

    [[nodiscard]] const TLogInfo& info_api() const noexcept {
        return log_info_;
    }

    [[nodiscard]] TLogWarn& warn_api() noexcept {
        return log_warn_;
    }

    [[nodiscard]] const TLogWarn& warn_api() const noexcept {
        return log_warn_;
    }

    [[nodiscard]] TLogError& error_api() noexcept {
        return log_error_;
    }

    [[nodiscard]] const TLogError& error_api() const noexcept {
        return log_error_;
    }

    [[nodiscard]] TLogFatal& fatal_api() noexcept {
        return log_fatal_;
    }

    [[nodiscard]] const TLogFatal& fatal_api() const noexcept {
        return log_fatal_;
    }

    [[nodiscard]] TLogTrace& trace_api() noexcept {
        return log_trace_;
    }

    [[nodiscard]] const TLogTrace& trace_api() const noexcept {
        return log_trace_;
    }

    [[nodiscard]] auto LogDebug(typename TLogDebug::ContentType content) const {
        return log_debug_.AcceptLog(std::move(content));
    }

    [[nodiscard]] auto LogInfo(typename TLogInfo::ContentType content) const {
        return log_info_.AcceptLog(std::move(content));
    }

    [[nodiscard]] auto LogWarn(typename TLogWarn::ContentType content) const {
        return log_warn_.AcceptLog(std::move(content));
    }

    [[nodiscard]] auto LogError(typename TLogError::ContentType content) const {
        return log_error_.AcceptLog(std::move(content));
    }

    [[nodiscard]] auto LogFatal(typename TLogFatal::ContentType content) const {
        return log_fatal_.AcceptLog(std::move(content));
    }

    [[nodiscard]] auto LogTrace(typename TLogTrace::ContentType content) const {
        return log_trace_.AcceptLog(std::move(content));
    }

private:
    TLogDebug log_debug_{};
    TLogInfo log_info_{};
    TLogWarn log_warn_{};
    TLogError log_error_{};
    TLogFatal log_fatal_{};
    TLogTrace log_trace_{};
};

}  // namespace logging_system::M_Surfaces