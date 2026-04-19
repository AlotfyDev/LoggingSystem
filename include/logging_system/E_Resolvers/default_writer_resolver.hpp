#pragma once

/*
------------------------------------------------------------------------------
E_Resolvers/default_writer_resolver.hpp

Role in the architecture
------------------------
DefaultWriterResolver is the minimal write-side access/reference pre-computation
component.

It answers one narrow question:
    "Given a newly admitted record and its level binding inside the shared state
     core, what is the write-side target descriptor for that record?"

Why this file exists in this batch
----------------------------------
Before pipeline-specific resolver bindings can exist, the system needs a
default reusable writer-side resolver that can shape write-target metadata from
the state core, without performing the write itself.

Current minimal scope
---------------------
This file currently provides:
- a WriteTargetDescriptor containing:
  - record_id
  - level_key
  - slot_state
  - membership flag
- a resolve_write_target(...) operation
- a build_handoff_event(...) operation for the next layer

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- richer target descriptors
- pipeline-local write target annotations
- route-hint augmentation
- multi-tenant/composite partition support
- stronger trait-based extraction of record identity
- optional policy-aware write readiness

What should NOT live here
-------------------------
This file must NOT:
- write into storage
- mutate state
- build envelopes or records
- validate schema/content
- choose adapters
- execute dispatch
- become a handler/orchestrator

Design rule
-----------
This file computes write-side reference metadata only.
It is a shaping layer, not an execution layer.
------------------------------------------------------------------------------
*/

#include <string>

#include "logging_system/H_State/log_container_module.hpp"
#include "logging_system/H_State/slot_lifecycle.hpp"

namespace logging_system::E_Resolvers {

struct WriteTargetDescriptor final {
    std::string record_id{};
    std::string level_key{};
    logging_system::H_State::SlotLifecycle::State slot_state{
        logging_system::H_State::SlotLifecycle::State::New};
    bool level_membership_confirmed{false};
};

struct WriteHandoffEvent final {
    WriteTargetDescriptor target{};
    bool ready{false};
};

struct DefaultWriterResolver final {
    template <typename TRecord>
    static WriteTargetDescriptor resolve_write_target(
        const logging_system::H_State::LogContainerModule<TRecord>& module,
        const std::string& level_key,
        const TRecord& record) {
        return WriteTargetDescriptor{
            record.record_id,
            level_key,
            module.slot_lifecycle().get_state(record.record_id),
            module.level_containers().contains_record(level_key, record.record_id)
        };
    }

    static WriteHandoffEvent build_handoff_event(
        const WriteTargetDescriptor& target) {
        return WriteHandoffEvent{
            target,
            target.level_membership_confirmed
        };
    }
};

}  // namespace logging_system::E_Resolvers