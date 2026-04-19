#pragma once

/*
------------------------------------------------------------------------------
L_Level_api/log_error.hpp

Reference Version
-----------------
ERROR_SLICE_BASELINE_V1

Role in the architecture
------------------------
LogError is the finalized thin dedicated ERROR-level entrypoint over the ERROR
pipeline slice.

It answers narrow questions such as:
    "How does external code submit work into the ERROR pipeline without passing
     through a generic shared service.log(level=...) convergence point?"
    "How does the ERROR pipeline expose both a direct runnable helper path and a
     state-admission-aware path through one thin per-level entry surface?"

Why this file exists in this stage
----------------------------------
The ERROR slice now has:
- ERROR preparation binding
- ERROR resolver binding
- ERROR dispatch binding
- ERROR repository route
- ERROR pipeline binding
- a runnable pipeline runner
- a minimal adapter boundary
- a consuming surface above it

At this stage, the ERROR entrypoint must evolve from:
- a very thin helper wrapper

into:
- a finalized thin ERROR-level entry that reflects the now-upgraded runner and
  the admitted-runtime path of the slice.

Current minimal scope
---------------------
This file currently provides:
- `LogError`
- fixed `level_key()`
- `resolve_default_route()`
- `run_single(...)`
- `admit_and_run(...)`

The intended usage split is:
- `run_single(...)`
    direct record-driven runnable helper path
- `admit_and_run(...)`
    state-admission-aware ERROR path for the finalized slice

This closes the dedicated ERROR entrypoint for the current slice without
reintroducing:
- generic shared level switching
- a monolithic logging service root
- central service.log(...) convergence

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- raw-content submission helpers that invoke preparation/stabilization before
  admission
- ERROR-specific convenience overloads
- CLI/application-oriented helper aliases
- integration hooks for broader consuming surfaces
- stronger compile-time validation against pipeline-local contracts

What should NOT live here
-------------------------
This file must NOT:
- become a shared level multiplexer
- own shared state
- own adapter registry logic
- own governance/configuration
- implement pipeline internals directly
- collapse back into generic service.log(...) routing

Design rule
-----------
This file is a thin per-level entrypoint only.
Its job is to make the ERROR pipeline directly reachable as its own dedicated
path, while delegating actual pipeline behavior to the ERROR pipeline runner.
------------------------------------------------------------------------------
*/

#include <optional>
#include <string>

#include "logging_system/K_Pipelines/error_pipeline_binding.hpp"
#include "logging_system/K_Pipelines/pipeline_runner.hpp"

namespace logging_system::L_Level_api {

struct LogError final {
    using PipelineBinding = logging_system::K_Pipelines::ErrorPipelineBinding;
    using Runner = logging_system::K_Pipelines::PipelineRunner<PipelineBinding>;

    static constexpr const char* level_key() noexcept {
        return "ERROR";
    }

    static auto resolve_default_route() {
        return Runner::resolve_default_route();
    }

    template <typename TModule, typename TRecord, typename TAdapter>
    static auto run_single(
        const TModule& module,
        const TRecord& record,
        TAdapter& adapter,
        const std::optional<std::string>& round_id = std::nullopt) {
        return Runner::run_single(
            module,
            level_key(),
            record,
            adapter,
            round_id);
    }

    template <typename TModule, typename TRecord, typename TAdapter>
    static auto admit_and_run(
        TModule& module,
        const TRecord& record,
        TAdapter& adapter,
        const std::optional<std::string>& round_id = std::nullopt) {
        return Runner::admit_and_run(
            module,
            level_key(),
            record,
            adapter,
            round_id);
    }
};

}  // namespace logging_system::L_Level_api