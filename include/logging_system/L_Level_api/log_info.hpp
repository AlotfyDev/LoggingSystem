#pragma once

/*
------------------------------------------------------------------------------
L_Level_api/log_info.hpp

Role in the architecture
------------------------
LogInfo is the thin dedicated INFO-level entrypoint into the runnable INFO
pipeline.

It answers one narrow question:
    "How does external code enter the INFO pipeline directly, without passing
     through a central shared service.log(level=...) convergence point?"

Why this file exists in this stage
----------------------------------
The INFO slice now has:
- INFO preparation binding
- INFO resolver binding
- INFO dispatch binding
- INFO repository route
- INFO pipeline binding
- a generic pipeline façade
- a runnable pipeline runner
- a minimal adapter boundary

The next dependency-first step is therefore a thin INFO entrypoint that binds
external callers directly to the INFO pipeline's runnable path.

Current minimal scope
---------------------
This file currently provides:
- `LogInfo`
- fixed `level_key()`
- `resolve_default_route()`
- `run_single(module, record, adapter, round_id)`

This is intentionally a thin entrypoint over the INFO pipeline.
It does not introduce shared level switching or central runtime convergence.

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- preparation-stage raw-content submission helpers
- INFO-specific convenience overloads
- consuming-surface integration hooks
- CLI integration helpers
- pipeline-local policy-aware entry helpers

What should NOT live here
-------------------------
This file must NOT:
- become a shared level multiplexer
- own shared state
- own adapter registry logic
- own governance/configuration
- collapse back into generic service.log(...) routing

Design rule
-----------
This file is a thin per-level entrypoint only.
Its job is to make the INFO pipeline directly callable as its own dedicated
path.
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
};

}  // namespace logging_system::L_Level_api