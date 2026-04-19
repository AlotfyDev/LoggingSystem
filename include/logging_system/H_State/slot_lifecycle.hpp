#pragma once

/*
------------------------------------------------------------------------------
H_State/slot_lifecycle.hpp

Role in the architecture
------------------------
SlotLifecycle is the minimal per-record slot state vocabulary and storage.

It answers one narrow question:
    "What is the current runtime state of this record slot?"

Why this file exists in P1
--------------------------
Resolvers and dispatch logic need state semantics before they can compute
meaningful access or dispatch decisions. Level membership alone is not enough.
The system must distinguish at least between:
- newly admitted
- pending
- dispatching
- dispatched
- failed
- evicted

Current minimal scope
---------------------
This file currently provides:
- a minimal State enum
- state storage by record id
- ensure/get/set/erase operations
- snapshot export
- textual conversion helper

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- explicit transition validation rules
- richer substates / reason codes
- timestamps per transition
- transition history
- slot annotations
- lifecycle policy integration
- retention/eviction coordination hooks
- thread-safety policy at a higher integration layer

What should NOT live here
-------------------------
This file must NOT own:
- full record objects
- pending queue ordering
- level partition index
- listeners
- dispatch execution
- schema or payload knowledge
- adapter/governance concerns

Design rule
-----------
This file is lifecycle vocabulary + lifecycle state storage.
It is subordinate to LogContainerModule and should remain small and explicit.
------------------------------------------------------------------------------
*/

#include <string>
#include <unordered_map>
#include <vector>

namespace logging_system::H_State {

class SlotLifecycle final {
public:
    enum class State {
        New,
        Pending,
        Dispatching,
        Dispatched,
        Failed,
        Evicted
    };

    using RecordId = std::string;

    struct SnapshotEntry final {
        RecordId record_id{};
        State state{State::New};
    };

    SlotLifecycle() = default;

    void ensure_slot(const RecordId& record_id) {
        states_.emplace(record_id, State::New);
    }

    void set_pending(const RecordId& record_id) {
        states_[record_id] = State::Pending;
    }

    void set_dispatching(const RecordId& record_id) {
        states_[record_id] = State::Dispatching;
    }

    void set_dispatched(const RecordId& record_id) {
        states_[record_id] = State::Dispatched;
    }

    void set_failed(const RecordId& record_id) {
        states_[record_id] = State::Failed;
    }

    void set_evicted(const RecordId& record_id) {
        states_[record_id] = State::Evicted;
    }

    bool has_slot(const RecordId& record_id) const {
        return states_.find(record_id) != states_.end();
    }

    State get_state(const RecordId& record_id) const {
        const auto it = states_.find(record_id);
        if (it == states_.end()) {
            return State::New;
        }
        return it->second;
    }

    bool erase_slot(const RecordId& record_id) {
        return states_.erase(record_id) > 0;
    }

    std::vector<SnapshotEntry> snapshot() const {
        std::vector<SnapshotEntry> out;
        out.reserve(states_.size());

        for (const auto& entry : states_) {
            out.push_back(SnapshotEntry{entry.first, entry.second});
        }

        return out;
    }

    static const char* to_string(State state) noexcept {
        switch (state) {
            case State::New:         return "NEW";
            case State::Pending:     return "PENDING";
            case State::Dispatching: return "DISPATCHING";
            case State::Dispatched:  return "DISPATCHED";
            case State::Failed:      return "FAILED";
            case State::Evicted:     return "EVICTED";
        }
        return "UNKNOWN";
    }

private:
    std::unordered_map<RecordId, State> states_{};
};

}  // namespace logging_system::H_State