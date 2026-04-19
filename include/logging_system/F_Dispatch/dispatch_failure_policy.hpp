#pragma once

/*
------------------------------------------------------------------------------
F_Dispatch/dispatch_failure_policy.hpp

Role in the architecture
------------------------
DispatchFailurePolicy defines minimal failure-handling vocabulary for dispatch
attempts.

It answers narrow questions such as:
    "When dispatch fails, what policy mode should the next layer apply?"
    "Should failed work be requeued, marked failed, or abort the batch?"

Why this file exists in this batch
----------------------------------
Execution layers should not bury failure semantics in ad hoc branches.
They need a stable policy object that can later be bound per pipeline while
remaining independent from the executor implementation itself.

Current minimal scope
---------------------
This file currently provides:
- a FailureMode enum
- a DispatchFailurePolicy struct containing:
  - mode
  - max_retry_attempts

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- retry delay hints
- exponential backoff hints
- partial-batch vs whole-batch handling
- policy-aware reporting hooks
- pipeline-local escalation modes
- dead-letter / quarantine hints

What should NOT live here
-------------------------
This file must NOT:
- requeue records itself
- mark state itself
- execute retries itself
- emit adapter calls
- own scheduler/timer behavior

Design rule
-----------
This file provides failure-policy vocabulary only.
It is descriptive input to executors/coordinators, not an execution engine.
------------------------------------------------------------------------------
*/

#include <cstddef>

namespace logging_system::F_Dispatch {

enum class FailureMode {
    Requeue,
    MarkFailed,
    AbortBatch
};

struct DispatchFailurePolicy final {
    FailureMode mode{FailureMode::Requeue};
    std::size_t max_retry_attempts{0};
};

}  // namespace logging_system::F_Dispatch