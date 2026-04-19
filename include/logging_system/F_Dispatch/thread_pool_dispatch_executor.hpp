#pragma once

/*
------------------------------------------------------------------------------
F_Dispatch/thread_pool_dispatch_executor.hpp

Role in the architecture
------------------------
ThreadPoolDispatchExecutor is the minimal execution strategy façade for dispatch
batches.

Important note for this batch
-----------------------------
Despite its name, this P-minimal version does NOT implement a real thread pool.
The name is preserved because:
- it matches the intended architectural role in the larger design
- later expansions can grow this file into a true thread-pool-backed executor
  without changing the file's place in the architecture

Current minimal scope
---------------------
This file currently provides:
- an ExecutionResult summary containing:
  - attempted
  - succeeded
  - failed
- a static execute_batch(...) operation
- the operation iterates over a batch of dispatch contexts and calls a provided
  emission bridge/callable

Why this file exists in this batch
----------------------------------
The dispatch layer needs a minimal executable shape before specialized bindings
and pipeline runners can be completed. This file provides that shape without
prematurely building a full scheduler/pool implementation.

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- real worker-thread pool management
- bounded task queue integration
- async futures/promises
- policy-aware scheduling
- retry and backoff coordination
- instrumentation/timing
- cancellation/shutdown control
- partial-batch execution strategies

What should NOT live here
-------------------------
This file must NOT:
- own adapter registry logic
- compute resolver outputs
- own state-core mutation
- requeue/commit records directly
- become a governance/configuration layer

Design rule
-----------
This file is an execution-strategy layer.
In this minimal version it provides the execution shape, not the final
high-throughput concurrency machinery.
------------------------------------------------------------------------------
*/

#include <cstddef>
#include <vector>

#include "logging_system/F_Dispatch/dispatch_failure_policy.hpp"
#include "logging_system/F_Dispatch/queue_policy.hpp"

namespace logging_system::F_Dispatch {

struct ExecutionResult final {
    std::size_t attempted{0};
    std::size_t succeeded{0};
    std::size_t failed{0};
};

struct ThreadPoolDispatchExecutor final {
    template <typename TAdapter, typename TDispatchContext, typename TEmissionBridge>
    static ExecutionResult execute_batch(
        TAdapter& adapter,
        const std::vector<TDispatchContext>& batch,
        TEmissionBridge emission_bridge,
        const QueuePolicy& queue_policy = {},
        const DispatchFailurePolicy& failure_policy = {}) {
        (void)failure_policy;

        ExecutionResult result{};

        const std::size_t batch_limit =
            (queue_policy.max_batch_size < batch.size())
                ? queue_policy.max_batch_size
                : batch.size();

        for (std::size_t i = 0; i < batch_limit; ++i) {
            ++result.attempted;

            try {
                (void)emission_bridge.emit(adapter, batch[i]);
                ++result.succeeded;
            } catch (...) {
                ++result.failed;
            }
        }

        return result;
    }
};

}  // namespace logging_system::F_Dispatch