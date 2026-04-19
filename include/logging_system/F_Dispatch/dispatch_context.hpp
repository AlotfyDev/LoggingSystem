#pragma once

/*
------------------------------------------------------------------------------
F_Dispatch/dispatch_context.hpp

Role in the architecture
------------------------
DispatchContext is the minimal execution-facing carrier for one dispatch unit.

It answers one narrow question:
    "What does the execution/emission layer need to know about this dispatchable
     record, after resolver pre-computation has already happened?"

Why this file exists in this batch
----------------------------------
Execution and adapter-emission code should not receive loose argument lists such
as:
- candidate
- receiver binding
- round id
- level key
- route key
- record id
as separate parameters everywhere.

This file provides a single minimal context object that carries the dispatch
descriptors already computed by resolver layers.

Current minimal scope
---------------------
This file currently provides:
- a DispatchContext template that stores:
  - record
  - dispatch candidate descriptor
  - dispatch receiver binding descriptor
  - optional round_id

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- batch-local execution metadata
- lease/binding snapshot fragments
- queue/backpressure hints
- correlation ids
- retry attempt counters
- adapter family hints
- execution timing metadata

What should NOT live here
-------------------------
This file must NOT:
- compute routes
- compute resolver outputs
- execute adapter emission
- own queue semantics
- own state storage
- own governance/configuration logic

Design rule
-----------
This file is a dispatch carrier only.
It packages already-resolved metadata for the execution/emission layer.
------------------------------------------------------------------------------
*/

#include <optional>
#include <string>
#include <utility>

namespace logging_system::F_Dispatch {

template <typename TRecord, typename TDispatchCandidate, typename TReceiverBinding>
struct DispatchContext final {
    using RecordType = TRecord;
    using DispatchCandidateType = TDispatchCandidate;
    using ReceiverBindingType = TReceiverBinding;

    TRecord record{};
    TDispatchCandidate candidate{};
    TReceiverBinding receiver_binding{};
    std::optional<std::string> round_id{std::nullopt};

    DispatchContext() = default;

    DispatchContext(
        TRecord record_in,
        TDispatchCandidate candidate_in,
        TReceiverBinding receiver_binding_in,
        std::optional<std::string> round_id_in = std::nullopt)
        : record(std::move(record_in)),
          candidate(std::move(candidate_in)),
          receiver_binding(std::move(receiver_binding_in)),
          round_id(std::move(round_id_in)) {}
};

}  // namespace logging_system::F_Dispatch