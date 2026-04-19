#pragma once

/*
------------------------------------------------------------------------------
C_Contracts/dispatch_executor_concept.hpp

Role in the architecture
------------------------
This file provides the architectural "concept" for dispatch-executor-like types.

It answers one narrow question:
    "Can this type execute a dispatch batch according to the minimal dispatch
     execution shape of the architecture?"

Why this file exists in S1
--------------------------
The INFO slice now has a minimal dispatch layer, including a default executor.
Before pipeline runners and higher layers rely on executors more deeply, the
architecture needs a compile-time contract vocabulary for:
- execute_batch(adapter, batch, emission_bridge, queue_policy, failure_policy)

Current minimal scope
---------------------
This file currently checks whether a type provides:
- `execute_batch(adapter, batch, emission_bridge, queue_policy, failure_policy)`

The exact result type is intentionally not overconstrained yet.

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- richer execution-result checks
- async-capable executor traits
- policy-aware executor specializations
- binding-level integration assertions
- capability-aware scheduling helpers

What should NOT live here
-------------------------
This file must NOT:
- implement execution
- own queues
- own adapters
- resolve routes
- mutate state directly
- become a runtime validator

Design rule
-----------
This file provides compile-time shape validation only.
It checks whether a type exposes dispatch-executor-like behavior.
------------------------------------------------------------------------------
*/

#include <type_traits>
#include <utility>

namespace logging_system::C_Contracts {

template <typename T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

template <
    typename TExecutor,
    typename TAdapter,
    typename TBatch,
    typename TEmissionBridge,
    typename TQueuePolicy,
    typename TFailurePolicy,
    typename = void>
struct is_dispatch_executor : std::false_type {};

template <
    typename TExecutor,
    typename TAdapter,
    typename TBatch,
    typename TEmissionBridge,
    typename TQueuePolicy,
    typename TFailurePolicy>
struct is_dispatch_executor<
    TExecutor,
    TAdapter,
    TBatch,
    TEmissionBridge,
    TQueuePolicy,
    TFailurePolicy,
    std::void_t<
        decltype(remove_cvref_t<TExecutor>::execute_batch(
            std::declval<TAdapter&>(),
            std::declval<const TBatch&>(),
            std::declval<TEmissionBridge>(),
            std::declval<const TQueuePolicy&>(),
            std::declval<const TFailurePolicy&>()))>>
    : std::true_type {};

template <
    typename TExecutor,
    typename TAdapter,
    typename TBatch,
    typename TEmissionBridge,
    typename TQueuePolicy,
    typename TFailurePolicy>
inline constexpr bool is_dispatch_executor_v =
    is_dispatch_executor<
        TExecutor,
        TAdapter,
        TBatch,
        TEmissionBridge,
        TQueuePolicy,
        TFailurePolicy>::value;

template <
    typename TExecutor,
    typename TAdapter,
    typename TBatch,
    typename TEmissionBridge,
    typename TQueuePolicy,
    typename TFailurePolicy>
struct dispatch_executor_concept
    : std::bool_constant<
          is_dispatch_executor_v<
              TExecutor,
              TAdapter,
              TBatch,
              TEmissionBridge,
              TQueuePolicy,
              TFailurePolicy>> {};

template <
    typename TExecutor,
    typename TAdapter,
    typename TBatch,
    typename TEmissionBridge,
    typename TQueuePolicy,
    typename TFailurePolicy>
inline constexpr bool dispatch_executor_concept_v =
    dispatch_executor_concept<
        TExecutor,
        TAdapter,
        TBatch,
        TEmissionBridge,
        TQueuePolicy,
        TFailurePolicy>::value;

template <
    typename TExecutor,
    typename TAdapter,
    typename TBatch,
    typename TEmissionBridge,
    typename TQueuePolicy,
    typename TFailurePolicy>
using require_dispatch_executor_t =
    std::enable_if_t<
        dispatch_executor_concept_v<
            TExecutor,
            TAdapter,
            TBatch,
            TEmissionBridge,
            TQueuePolicy,
            TFailurePolicy>,
        int>;

}  // namespace logging_system::C_Contracts