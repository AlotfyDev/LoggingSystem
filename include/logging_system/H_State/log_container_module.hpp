#pragma once

/*
------------------------------------------------------------------------------
H_State/log_container_module.hpp

Role in the architecture
------------------------
LogContainerModule is the minimal shared state admission and coordination module
for records inside the runtime core.

It answers the following state-core questions:
- How are records admitted into runtime state?
- Which records are currently pending dispatch?
- How do we drain pending records for dispatch attempts?
- How do we requeue a failed batch?
- How do we commit a successfully dispatched batch?
- How do level indexing and lifecycle state stay synchronized?

Why this file exists in P1
--------------------------
Resolvers should not be built against vague or hypothetical storage behavior.
Before resolver defaults exist, the system needs a minimal but real shared
module that defines:
- record admission
- pending ordering
- lookup by record id
- level partition ownership
- lifecycle synchronization

Current minimal scope
---------------------
This file currently provides:
- record admission through enqueue_pending(level_key, record)
- pending batch extraction through drain_pending(...)
- failed batch rollback through requeue_pending_front(...)
- successful batch confirmation through commit_dispatched(...)
- record lookup and listing helpers
- listener subscription hooks
- access to subordinate LevelContainers and SlotLifecycle

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- retention and eviction integration
- richer inspection / state snapshot reporting
- explicit pending queue policy extraction
- listener isolation/failure accounting
- lifecycle transition validation coordination
- thread-safety / locking policy
- pagination and bounded storage policies
- more explicit dispatch lease coordination hooks
- state snapshot materialization for readonly/reporting layers

What should NOT live here
-------------------------
This file must NOT become:
- a resolver implementation
- a dispatch executor
- an adapter boundary
- a governance/configuration service
- a level API facade
- a monolithic system root

Design rule
-----------
This file is the shared runtime state coordinator for admitted records.
It owns record storage and pending coordination, while delegating:
- level membership indexing to LevelContainers
- slot-state vocabulary/storage to SlotLifecycle
------------------------------------------------------------------------------
*/

#include <cstddef>
#include <deque>
#include <functional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "logging_system/H_State/level_containers.hpp"
#include "logging_system/H_State/slot_lifecycle.hpp"

namespace logging_system::H_State {

template <typename TRecord>
class LogContainerModule final {
public:
    using RecordType = TRecord;
    using RecordId = std::string;
    using LevelKey = std::string;

    using ListenerId = std::string;
    using Listener = std::function<void(const RecordType&)>;

    struct Inspection final {
        std::size_t stored_records{0};
        std::size_t pending_records{0};
        std::size_t level_partition_count{0};
        std::size_t listener_count{0};
    };

    LogContainerModule() = default;

    void enqueue_pending(const LevelKey& level_key, const RecordType& record) {
        const auto& record_id = record.record_id;

        records_by_id_[record_id] = record;
        levels_by_record_id_[record_id] = level_key;
        pending_order_.push_back(record_id);

        level_containers_.add_record(level_key, record_id);
        slot_lifecycle_.ensure_slot(record_id);
        slot_lifecycle_.set_pending(record_id);

        notify_listeners(record);
    }

    std::vector<RecordType> drain_pending(
        std::size_t max_count = static_cast<std::size_t>(-1)) {
        std::vector<RecordType> out;

        const auto count =
            (max_count < pending_order_.size()) ? max_count : pending_order_.size();

        out.reserve(count);

        for (std::size_t i = 0; i < count; ++i) {
            const auto record_id = pending_order_.front();
            pending_order_.pop_front();

            auto record_it = records_by_id_.find(record_id);
            if (record_it == records_by_id_.end()) {
                continue;
            }

            slot_lifecycle_.set_dispatching(record_id);
            out.push_back(record_it->second);
        }

        return out;
    }

    void requeue_pending_front(const std::vector<RecordType>& records) {
        for (auto it = records.rbegin(); it != records.rend(); ++it) {
            pending_order_.push_front(it->record_id);
            slot_lifecycle_.set_pending(it->record_id);
        }
    }

    void commit_dispatched(const std::vector<RecordType>& records) {
        for (const auto& record : records) {
            slot_lifecycle_.set_dispatched(record.record_id);
        }
    }

    void mark_failed(const std::vector<RecordType>& records) {
        for (const auto& record : records) {
            slot_lifecycle_.set_failed(record.record_id);
        }
    }

    const RecordType* find_record(const RecordId& record_id) const {
        const auto it = records_by_id_.find(record_id);
        if (it == records_by_id_.end()) {
            return nullptr;
        }
        return &it->second;
    }

    std::vector<RecordType> list_records_by_level(const LevelKey& level_key) const {
        std::vector<RecordType> out;
        const auto record_ids = level_containers_.list_records(level_key);
        out.reserve(record_ids.size());

        for (const auto& record_id : record_ids) {
            const auto it = records_by_id_.find(record_id);
            if (it != records_by_id_.end()) {
                out.push_back(it->second);
            }
        }

        return out;
    }

    std::vector<RecordType> list_all_records() const {
        std::vector<RecordType> out;
        out.reserve(records_by_id_.size());

        for (const auto& entry : records_by_id_) {
            out.push_back(entry.second);
        }

        return out;
    }

    void subscribe_listener(const ListenerId& listener_id, Listener listener) {
        listeners_[listener_id] = std::move(listener);
    }

    bool unsubscribe_listener(const ListenerId& listener_id) {
        return listeners_.erase(listener_id) > 0;
    }

    Inspection inspect() const {
        return Inspection{
            records_by_id_.size(),
            pending_order_.size(),
            level_containers_.list_levels().size(),
            listeners_.size()
        };
    }

    const LevelContainers& level_containers() const noexcept {
        return level_containers_;
    }

    LevelContainers& level_containers() noexcept {
        return level_containers_;
    }

    const SlotLifecycle& slot_lifecycle() const noexcept {
        return slot_lifecycle_;
    }

    SlotLifecycle& slot_lifecycle() noexcept {
        return slot_lifecycle_;
    }

private:
    void notify_listeners(const RecordType& record) const {
        for (const auto& entry : listeners_) {
            entry.second(record);
        }
    }

    std::unordered_map<RecordId, RecordType> records_by_id_{};
    std::unordered_map<RecordId, LevelKey> levels_by_record_id_{};
    std::deque<RecordId> pending_order_{};

    std::unordered_map<ListenerId, Listener> listeners_{};

    LevelContainers level_containers_{};
    SlotLifecycle slot_lifecycle_{};
};

}  // namespace logging_system::H_State