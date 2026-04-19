#pragma once

/*
------------------------------------------------------------------------------
F_Dispatch/debug_dispatch_binding.hpp

Reference Version
-----------------
DEBUG_SLICE_BASELINE_V1

Role in the architecture
------------------------
DebugDispatchBinding is the DEBUG-pipeline specialization of the generic dispatch
binding family.

It answers one narrow question:
    "Which dispatch-layer components constitute the dispatch stack for the DEBUG
     pipeline right now?"

Why this file exists in this batch
----------------------------------
Once dispatch vocabulary and default execution/emission components exist, the
pipeline needs an explicit dispatch binding that states:
- which executor it uses
- which queue policy type it uses
- which failure policy type it uses
- which adapter emission bridge it uses

This keeps dispatch specialization explicit at the binding level, even when the
current implementations are still default/shared.

Current minimal scope
---------------------
This file currently binds:
- ThreadPoolDispatchExecutor
- QueuePolicy
- DispatchFailurePolicy
- DefaultAdapterEmission

into:
- DebugDispatchBinding

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- DEBUG-specific queue policy types
- DEBUG-specific failure policy types
- specialized adapter emission bridges
- stronger compile-time validation against dispatch contracts
- policy-bound executor variants

What should NOT live here
-------------------------
This file must NOT:
- implement execution logic
- implement adapter emission logic
- own state mutation
- own route computation
- become a pipeline orchestrator

Design rule
-----------
This file is a binding alias only.
Its job is to make pipeline-local dispatch composition explicit.
------------------------------------------------------------------------------
*/

#include "logging_system/A_Core/dispatch_binding.hpp"

#include "logging_system/F_Dispatch/default_adapter_emission.hpp"
#include "logging_system/F_Dispatch/dispatch_failure_policy.hpp"
#include "logging_system/F_Dispatch/queue_policy.hpp"
#include "logging_system/F_Dispatch/thread_pool_dispatch_executor.hpp"

namespace logging_system::F_Dispatch {

using DebugDispatchBinding = logging_system::A_Core::DispatchBinding<
    ThreadPoolDispatchExecutor,
    QueuePolicy,
    DispatchFailurePolicy,
    DefaultAdapterEmission>;

}  // namespace logging_system::F_Dispatch