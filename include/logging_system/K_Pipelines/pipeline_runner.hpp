#pragma once

/*

Very important design note:

3) Intentional Assumption at this Stage

To make the slice runnable now, I'll make this version of the runner work on:

TRecord already available
level_key comes from LogInfo
module is LogContainerModule<TRecord>
adapter is any adapter-like object, such as NoOpAdapter

This means:

This isn't yet the full raw-content-to-record pipeline, but it is a record-to-dispatch runnable path, and that's enough to properly close the first runnable slice.



------------------------------------------------------------------------------
K_Pipelines/pipeline_runner.hpp

Role in the architecture
------------------------
PipelineRunner is the minimal runnable execution unit for one concrete pipeline
binding.

It answers narrow questions such as:
    "How does one pipeline binding execute its own write-side and dispatch-side
     path without collapsing back into a central service?"
    "How can a per-pipeline thin API trigger a runnable path using only the
     components bound into that pipeline?"

Why this file exists in this stage
----------------------------------
The INFO slice already has:
- preparation binding
- resolver binding
- dispatch binding
- repository route
- pipeline binding
- route vocabulary
- dispatch vocabulary
- adapter boundary
- a concrete NoOp adapter

The next dependency-first step is therefore to provide a minimal runnable
pipeline unit that:
- uses pipeline-local resolver components
- uses pipeline-local dispatch components
- constructs a dispatch context
- executes emission through a provided adapter-like object

Current minimal scope
---------------------
This file currently provides:
- `PipelineRunner<TPipelineBinding>`
- `run_single(module, level_key, record, adapter, round_id)`:
    - resolves write target
    - builds write handoff event
    - resolves dispatch candidate
    - resolves dispatch receiver binding
    - builds a one-item dispatch batch
    - executes the batch through the pipeline's bound dispatch stack
- `resolve_default_route()`

This is intentionally minimal and record-driven.
It closes the first runnable slice without prematurely introducing:
- full raw-content preparation
- full state mutation orchestration
- system-root composition

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- preparation-stage raw content submission
- record stabilization hooks
- state-admission integration
- batch building from pending drains
- commit/requeue feedback hooks
- queue/failure policy overrides
- pipeline-local instrumentation
- stronger compile-time contract assertions

What should NOT live here
-------------------------
This file must NOT:
- become a system root
- own shared state
- own adapter registry logic
- own governance/configuration
- collapse per-pipeline behavior into generic central convergence

Design rule
-----------
This file is a runnable per-pipeline execution unit.
It executes one pipeline using the components already bound into that pipeline,
but does not claim ownership of the whole system.
------------------------------------------------------------------------------
*/

#include <optional>
#include <string>
#include <vector>

#include "logging_system/F_Dispatch/dispatch_context.hpp"
#include "logging_system/K_Pipelines/ingest_pipeline_facade.hpp"

namespace logging_system::K_Pipelines {

template <typename TPipelineBinding>
struct PipelineRunner final {
    using Facade = IngestPipelineFacade<TPipelineBinding>;

    using Preparation = typename Facade::Preparation;
    using Resolver = typename Facade::Resolver;
    using Dispatch = typename Facade::Dispatch;
    using RepositoryRoute = typename Facade::RepositoryRoute;

    static RepositoryRoute resolve_default_route() {
        return Facade::make_default_route();
    }

    template <typename TModule, typename TRecord, typename TAdapter>
    static auto run_single(
        const TModule& module,
        const std::string& level_key,
        const TRecord& record,
        TAdapter& adapter,
        const std::optional<std::string>& round_id = std::nullopt) {
        const auto write_target =
            typename Resolver::WriterResolver::resolve_write_target(
                module,
                level_key,
                record);

        const auto write_handoff_event =
            typename Resolver::WriterResolver::build_handoff_event(write_target);

        (void)write_handoff_event;

        const auto dispatch_candidate =
            typename Resolver::DispatcherResolver::resolve_dispatch_candidate(
                module,
                level_key,
                record);

        const auto receiver_binding =
            typename Resolver::DispatcherResolver::resolve_dispatch_receiver_binding(
                dispatch_candidate);

        using DispatchContext = logging_system::F_Dispatch::DispatchContext<
            TRecord,
            decltype(dispatch_candidate),
            decltype(receiver_binding)>;

        std::vector<DispatchContext> batch{};
        batch.reserve(1);
        batch.emplace_back(
            record,
            dispatch_candidate,
            receiver_binding,
            round_id);

        return typename Dispatch::DispatchExecutor::execute_batch(
            adapter,
            batch,
            typename Dispatch::AdapterEmission{},
            typename Dispatch::QueuePolicy{},
            typename Dispatch::FailurePolicy{});
    }
};

}  // namespace logging_system::K_Pipelines