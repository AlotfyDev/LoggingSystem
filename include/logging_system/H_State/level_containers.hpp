#pragma once

/*
------------------------------------------------------------------------------
H_State/level_containers.hpp

Role in the architecture
------------------------
LevelContainers is the minimal partitioned index of record identifiers by
log-level (or level-like partition key).

This file exists to answer one narrow question:
    "Which record ids belong to which level bucket, and in what insertion order?"

Why this file exists in P1
--------------------------
Resolvers and later dispatch/state coordination should not infer level
membership from a monolithic storage blob. They need a dedicated level-aware
index structure to compute access paths, target buckets, and partition-local
membership.

Current minimal scope
---------------------
This file currently provides:
- insertion of record ids into a level bucket
- removal of record ids from a level bucket
- membership testing
- listing records for one level
- listing known levels
- size/count helpers
- clear/reset

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- stable iteration policies per level
- bounded level partitions
- policy-aware eviction hooks
- richer partition descriptors
- optional composite partition keys beyond simple level strings
- optional snapshot/export helpers
- optional thread-safety wrapper/policy at a higher layer

What should NOT live here
-------------------------
This file must NOT own:
- full record objects
- payload/schema knowledge
- lifecycle state
- pending/drain/requeue logic
- listeners
- dispatch execution
- retention policy behavior
- governance or adapter concerns

Design rule
-----------
This is an index-only component.
It is subordinate to LogContainerModule, not a system root.
------------------------------------------------------------------------------
*/

#include <deque>
#include <string>
#include <unordered_map>
#include <vector>

namespace logging_system::H_State {

class LevelContainers final {
public:
    using RecordId = std::string;
    using LevelKey = std::string;
    using RecordIdBucket = std::deque<RecordId>;

    LevelContainers() = default;

    void add_record(const LevelKey& level_key, const RecordId& record_id) {
        records_by_level_[level_key].push_back(record_id);
    }

    bool remove_record(const LevelKey& level_key, const RecordId& record_id) {
        auto level_it = records_by_level_.find(level_key);
        if (level_it == records_by_level_.end()) {
            return false;
        }

        auto& bucket = level_it->second;
        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if (*it == record_id) {
                bucket.erase(it);
                if (bucket.empty()) {
                    records_by_level_.erase(level_it);
                }
                return true;
            }
        }

        return false;
    }

    bool contains_record(const LevelKey& level_key, const RecordId& record_id) const {
        const auto level_it = records_by_level_.find(level_key);
        if (level_it == records_by_level_.end()) {
            return false;
        }

        const auto& bucket = level_it->second;
        for (const auto& existing : bucket) {
            if (existing == record_id) {
                return true;
            }
        }
        return false;
    }

    std::vector<RecordId> list_records(const LevelKey& level_key) const {
        std::vector<RecordId> out;

        const auto level_it = records_by_level_.find(level_key);
        if (level_it == records_by_level_.end()) {
            return out;
        }

        const auto& bucket = level_it->second;
        out.reserve(bucket.size());
        for (const auto& record_id : bucket) {
            out.push_back(record_id);
        }
        return out;
    }

    std::vector<LevelKey> list_levels() const {
        std::vector<LevelKey> out;
        out.reserve(records_by_level_.size());

        for (const auto& entry : records_by_level_) {
            out.push_back(entry.first);
        }

        return out;
    }

    std::size_t level_size(const LevelKey& level_key) const {
        const auto level_it = records_by_level_.find(level_key);
        if (level_it == records_by_level_.end()) {
            return 0;
        }
        return level_it->second.size();
    }

    std::size_t total_size() const {
        std::size_t total = 0;
        for (const auto& entry : records_by_level_) {
            total += entry.second.size();
        }
        return total;
    }

    void clear() {
        records_by_level_.clear();
    }

private:
    std::unordered_map<LevelKey, RecordIdBucket> records_by_level_{};
};

}  // namespace logging_system::H_State