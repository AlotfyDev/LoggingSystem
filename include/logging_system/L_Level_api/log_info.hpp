#pragma once




/*
------------------------------------------------------------------------------
L_Level_api/log_info.hpp

Reference Version
-----------------
INFO_SLICE_SYNC_BASELINE_V1

Role in the architecture
------------------------
LogInfo is the finalized thin dedicated INFO-level entrypoint over the INFO
pipeline slice.

It answers narrow questions such as:
    "How does external code submit work into the INFO pipeline without passing
     through a generic shared service.log(level=...) convergence point?"
    "How does the INFO pipeline expose both a direct runnable helper path and a
     state-admission-aware path through one thin per-level entry surface?"

Why this file exists in this stage
----------------------------------
The INFO slice now has:
- INFO preparation binding
- INFO resolver binding
- INFO dispatch binding
- INFO repository route
- INFO pipeline binding
- a runnable pipeline runner
- a minimal adapter boundary
- a consuming surface above it

At this stage, the INFO entrypoint must evolve from:
- a very thin helper wrapper

into:
- a finalized thin INFO-level entry that reflects the now-upgraded runner and
  the admitted-runtime path of the slice.

Current minimal scope
---------------------
This file currently provides:
- `LogInfo`
- fixed `level_key()`
- `resolve_default_route()`
- `run_single(...)`
- `admit_and_run(...)`

The intended usage split is:
- `run_single(...)`
    direct record-driven runnable helper path
- `admit_and_run(...)`
    state-admission-aware INFO path for the finalized slice

This closes the dedicated INFO entrypoint for the current slice without
reintroducing:
- generic shared level switching
- a monolithic logging service root
- central service.log(...) convergence

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- raw-content submission helpers that invoke preparation/stabilization before
  admission
- INFO-specific convenience overloads
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
Its job is to make the INFO pipeline directly reachable as its own dedicated
path, while delegating actual pipeline behavior to the INFO pipeline runner.
------------------------------------------------------------------------------
*/

#include <optional>
#include <string>

#include "logging_system/K_Pipelines/info_pipeline_binding.hpp"
#include "logging_system/K_Pipelines/pipeline_runner.hpp"

namespace logging_system::L_Level_api {

struct LogInfo final {
    using PipelineBinding = logging_system::K_Pipelines::InfoPipelineBinding;
    using Runner = logging_system::K_Pipelines::PipelineRunner<PipelineBinding>;

    static constexpr const char* level_key() noexcept {
        return "INFO";
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


















