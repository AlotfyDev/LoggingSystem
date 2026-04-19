#pragma once







/*
------------------------------------------------------------------------------
K_Pipelines/pipeline_runner.hpp

Very important design note:

3) Intentional Assumption at this Stage

To make the slice runnable now, I'll make this version of the runner work on:

TRecord already available
level_key comes from LogInfo
module is LogContainerModule<TRecord>
adapter is any adapter-like object, such as NoOpAdapter

This means:

This isn't yet the full raw-content-to-record pipeline, but it is a record-to-dispatch runnable path, and that's enough to properly close the first runnable slice.



Reference Version
-----------------
INFO_SLICE_SYNC_BASELINE_V1

Role in the architecture
------------------------
PipelineRunner is the minimal runnable execution unit for one concrete pipeline
binding.

It answers narrow questions such as:
    "How does one pipeline binding execute its own admitted-runtime path without
     collapsing back into a central service?"
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

At this stage, the pipeline runner must evolve from:
- record-to-dispatch helper

into:
- minimal admitted-runtime pipeline path

That means this file must now:
- accept a record into the shared state module
- drain pending work from the module
- resolve dispatch descriptors for drained records
- execute a dispatch batch through the bound dispatch stack
- feed the execution result back into the state module

Intentional assumption at this stage
------------------------------------
To make the slice runnable now, this version of the runner works on:
- TRecord already available
- level_key supplied by the per-level thin API (for example LogInfo)
- module being a LogContainerModule<TRecord>-like state module
- adapter being any adapter-like object such as NoOpAdapter

This means:
- this is not yet the full raw-content-to-record pipeline
- but it is a minimal admitted-runtime slice
- and that is enough to properly close the first reference slice

Current minimal scope
---------------------
This file currently provides:
- `run_single(...)`
    direct record-driven dispatch helper
- `admit_and_run(...)`
    minimal admitted-runtime path
- `resolve_default_route()`

The admitted-runtime path currently does the following:
1. enqueues the record into the shared state module
2. drains pending records
3. resolves dispatch candidate + receiver binding per record
4. builds dispatch contexts
5. executes the batch through the bound dispatch stack
6. applies minimal state feedback:
   - commit on full success
   - requeue or mark-failed on failure according to failure policy mode

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- raw-content preparation hooks
- envelope/record stabilization integration
- partial-batch result reconciliation
- richer policy-aware feedback handling
- retry/backoff integration
- per-record execution result accounting
- stronger compile-time contract assertions
- instrumentation and timing hooks

What should NOT live here
-------------------------
This file must NOT:
- become a system root
- own shared state
- own adapter registry logic
- own governance/configuration
- collapse per-pipeline behavior into generic central convergence
- absorb preparation/component implementations that belong elsewhere

Design rule
-----------
This file is a runnable per-pipeline execution unit.
It executes one pipeline using the components already bound into that pipeline,
while preserving per-pipeline specialization and shared-state boundaries.
------------------------------------------------------------------------------
*/

#include <optional>
#include <string>
#include <vector>

#include "logging_system/F_Dispatch/dispatch_context.hpp"
#include "logging_system/F_Dispatch/dispatch_failure_policy.hpp"
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

    template <typename TModule, typename TRecord, typename TAdapter>
    static auto admit_and_run(
        TModule& module,
        const std::string& level_key,
        const TRecord& record,
        TAdapter& adapter,
        const std::optional<std::string>& round_id = std::nullopt) {
        module.enqueue_pending(level_key, record);

        const auto drained_records = module.drain_pending(
            typename Dispatch::QueuePolicy{}.max_batch_size);

        using DispatchContext = logging_system::F_Dispatch::DispatchContext<
            TRecord,
            decltype(typename Resolver::DispatcherResolver::resolve_dispatch_candidate(
                module,
                level_key,
                record)),
            decltype(typename Resolver::DispatcherResolver::resolve_dispatch_receiver_binding(
                typename Resolver::DispatcherResolver::resolve_dispatch_candidate(
                    module,
                    level_key,
                    record)))>;

        std::vector<DispatchContext> batch{};
        batch.reserve(drained_records.size());

        for (const auto& drained_record : drained_records) {
            const auto dispatch_candidate =
                typename Resolver::DispatcherResolver::resolve_dispatch_candidate(
                    module,
                    level_key,
                    drained_record);

            const auto receiver_binding =
                typename Resolver::DispatcherResolver::resolve_dispatch_receiver_binding(
                    dispatch_candidate);

            batch.emplace_back(
                drained_record,
                dispatch_candidate,
                receiver_binding,
                round_id);
        }

        const auto result =
            typename Dispatch::DispatchExecutor::execute_batch(
                adapter,
                batch,
                typename Dispatch::AdapterEmission{},
                typename Dispatch::QueuePolicy{},
                typename Dispatch::FailurePolicy{});

        apply_state_feedback(
            module,
            drained_records,
            result,
            typename Dispatch::FailurePolicy{});

        return result;
    }

private:
    template <typename TModule, typename TRecord, typename TExecutionResult>
    static void apply_state_feedback(
        TModule& module,
        const std::vector<TRecord>& drained_records,
        const TExecutionResult& result,
        const logging_system::F_Dispatch::DispatchFailurePolicy& failure_policy) {
        if (drained_records.empty()) {
            return;
        }

        if (result.failed == 0) {
            module.commit_dispatched(drained_records);
            return;
        }

        switch (failure_policy.mode) {
            case logging_system::F_Dispatch::FailureMode::Requeue:
                module.requeue_pending_front(drained_records);
                break;

            case logging_system::F_Dispatch::FailureMode::MarkFailed:
                module.mark_failed(drained_records);
                break;

            case logging_system::F_Dispatch::FailureMode::AbortBatch:
                module.mark_failed(drained_records);
                break;
        }
    }
};

}  // namespace logging_system::K_Pipelines



