#pragma once

/*
------------------------------------------------------------------------------
E_Resolvers/default_dispatcher_resolver.hpp

Role in the architecture
------------------------
DefaultDispatcherResolver is the minimal dispatch-side access/reference
pre-computation component.

It answers two narrow questions:
    "What dispatch candidate descriptor corresponds to this pending record?"
    "What dispatch receiver/binding descriptor should the next layer receive?"

Why this file exists in this batch
----------------------------------
Dispatch defaults and later pipeline runners should not inspect raw state in an
ad hoc way. They need a reusable default resolver that translates state/batch
inputs into dispatch-facing metadata, without performing adapter emission or
execution orchestration.

Current minimal scope
---------------------
This file currently provides:
- a DispatchCandidateDescriptor containing:
  - record_id
  - level_key
  - slot_state
- a DispatchReceiverBindingDescriptor containing:
  - level_key
  - route_key
  - ready
- candidate resolution from state + record + level key
- receiver-binding shaping from a candidate descriptor

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- richer dispatch receiver metadata
- lease/binding hints
- adapter family hints
- queue/backpressure-related annotations
- route-context enrichment
- policy-aware readiness shaping

What should NOT live here
-------------------------
This file must NOT:
- execute dispatch
- choose the adapter instance
- mutate queue/state
- requeue/commit records
- validate schema/payload
- become an execution coordinator

Design rule
-----------
This file computes dispatch-side metadata only.
It prepares descriptors for execution layers, but does not execute them.
------------------------------------------------------------------------------
*/

#include <string>

#include "logging_system/H_State/log_container_module.hpp"
#include "logging_system/H_State/slot_lifecycle.hpp"

namespace logging_system::E_Resolvers {

struct DispatchCandidateDescriptor final {
    std::string record_id{};
    std::string level_key{};
    logging_system::H_State::SlotLifecycle::State slot_state{
        logging_system::H_State::SlotLifecycle::State::New};
};

struct DispatchReceiverBindingDescriptor final {
    std::string level_key{};
    std::string route_key{};
    bool ready{false};
};

struct DefaultDispatcherResolver final {
    template <typename TRecord>
    static DispatchCandidateDescriptor resolve_dispatch_candidate(
        const logging_system::H_State::LogContainerModule<TRecord>& module,
        const std::string& level_key,
        const TRecord& record) {
        return DispatchCandidateDescriptor{
            record.record_id,
            level_key,
            module.slot_lifecycle().get_state(record.record_id)
        };
    }

    static DispatchReceiverBindingDescriptor resolve_dispatch_receiver_binding(
        const DispatchCandidateDescriptor& candidate) {
        const bool ready =
            candidate.slot_state ==
            logging_system::H_State::SlotLifecycle::State::Dispatching;

        return DispatchReceiverBindingDescriptor{
            candidate.level_key,
            candidate.level_key,
            ready
        };
    }
};

}  // namespace logging_system::E_Resolvers