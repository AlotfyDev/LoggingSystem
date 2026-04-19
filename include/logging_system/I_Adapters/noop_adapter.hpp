#pragma once

/*
------------------------------------------------------------------------------
I_Adapters/noop_adapter.hpp

Role in the architecture
------------------------
NoOpAdapter is the minimal runnable adapter implementation for the current
slice.

It answers one narrow question:
    "What is the smallest adapter-like object that satisfies the adapter
     boundary and allows the pipeline to run end-to-end without introducing
     external dependencies?"

Why this file exists in S2
--------------------------
The INFO slice needs a concrete adapter-like type so that:
- adapter-emission bridging can be exercised
- dispatch executor shape can be exercised
- the pipeline runner can become runnable in the small
without requiring:
- adapter registry
- OpenTelemetry
- viewer integration
- external backend wiring

Current minimal scope
---------------------
This file currently provides:
- a `NoOpAdapter`
- a single `emit_signal(record)` operation
- a tiny `EmissionResult` return type

The default behavior is:
- accept the record
- report success
- perform no external side effects

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- counters/instrumentation
- optional capture buffers for tests
- richer emission result metadata
- adapter capability markers
- test-hook helpers
- integration with broader adapter concepts

What should NOT live here
-------------------------
This file must NOT:
- become an adapter registry
- own routing logic
- own dispatch logic
- own governance/configuration
- simulate real transport/backends beyond minimal testing utility

Design rule
-----------
This file is the minimal concrete adapter for runnable-slice closure.
It exists to satisfy the adapter boundary with the smallest possible behavior.
------------------------------------------------------------------------------
*/

#include <string>

namespace logging_system::I_Adapters {

struct EmissionResult final {
    bool accepted{true};
    std::string adapter_key{"noop"};
};

struct NoOpAdapter final {
    static constexpr const char* key() noexcept {
        return "noop";
    }

    template <typename TRecord>
    EmissionResult emit_signal(const TRecord&) const {
        return EmissionResult{true, key()};
    }
};

}  // namespace logging_system::I_Adapters