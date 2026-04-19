#pragma once

/*
------------------------------------------------------------------------------
F_Dispatch/queue_policy.hpp

Role in the architecture
------------------------
QueuePolicy defines minimal dispatch-queue vocabulary.

It answers narrow questions such as:
    "What is the expected batch size hint?"
    "What should happen when queue/backpressure pressure is encountered?"

Why this file exists in this batch
----------------------------------
Dispatch bindings and executors need a stable policy vocabulary even before
a richer queue implementation exists. Without this file, queue semantics would
either be buried inside executors or deferred as untyped booleans/integers.

Current minimal scope
---------------------
This file currently provides:
- an OverflowMode enum
- a QueuePolicy struct containing:
  - max_batch_size
  - overflow_mode

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- bounded queue capacity
- fairness policy
- queue priority hints
- time-based flush hints
- per-pipeline queue policy specialization
- pressure thresholds and instrumentation flags

What should NOT live here
-------------------------
This file must NOT:
- implement a queue
- own pending storage
- execute dispatch
- mutate state
- choose adapters
- become a scheduler

Design rule
-----------
This file provides queue policy vocabulary only.
It is intentionally descriptive, not operational.
------------------------------------------------------------------------------
*/

#include <cstddef>

namespace logging_system::F_Dispatch {

enum class OverflowMode {
    KeepAccepting,
    RejectNew,
    DropOldest
};

struct QueuePolicy final {
    std::size_t max_batch_size{64};
    OverflowMode overflow_mode{OverflowMode::KeepAccepting};
};

}  // namespace logging_system::F_Dispatch