#pragma once

/*
------------------------------------------------------------------------------
F_Dispatch/default_adapter_emission.hpp

Role in the architecture
------------------------
DefaultAdapterEmission is the minimal default bridge from dispatch context to an
adapter-like object that can emit a signal.

It answers one narrow question:
    "Given an adapter-like object and a prepared dispatch context, how do we
     perform the default emission call?"

Why this file exists in this batch
----------------------------------
Executors should not hardcode adapter call shapes inline. They should delegate
actual emission to a small reusable callable/bridge object.

Current minimal scope
---------------------
This file currently provides:
- a single static emit(...) operation
- the operation forwards to:
    adapter.emit_signal(context.record)

This intentionally assumes only the minimal default contract:
- the adapter-like object knows how to emit a record payload
- resolver and dispatch layers have already computed the metadata they need

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- emission overloads for richer adapter contracts
- use of dispatch context metadata beyond record itself
- adapter capability branching at a higher integration layer
- richer emitted result descriptors
- failure annotation helpers

What should NOT live here
-------------------------
This file must NOT:
- choose the adapter
- compute routes
- implement retries
- own failure policy application
- mutate state
- become an adapter registry

Design rule
-----------
This file is a default emission bridge only.
It forwards a prepared dispatch unit to an adapter-like object.
------------------------------------------------------------------------------
*/

#include <utility>

namespace logging_system::F_Dispatch {

struct DefaultAdapterEmission final {
    template <typename TAdapter, typename TDispatchContext>
    static auto emit(TAdapter& adapter, const TDispatchContext& context)
        -> decltype(adapter.emit_signal(context.record)) {
        return adapter.emit_signal(context.record);
    }
};

}  // namespace logging_system::F_Dispatch