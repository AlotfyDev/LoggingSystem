#pragma once

/*
------------------------------------------------------------------------------
E_Resolvers/default_readonly_resolver.hpp

Role in the architecture
------------------------
DefaultReadonlyResolver is the minimal readonly/query-side access/reference
pre-computation component.

It answers narrow questions such as:
    "What readonly descriptor represents one level partition?"
    "What readonly descriptor represents a whole-module inspection view?"

Why this file exists in this batch
----------------------------------
The resolver family should be complete and coherent:
- writer-side
- dispatch-side
- readonly-side

Even before advanced querying/projection logic exists, the system needs a
minimal readonly resolver that converts state access intention into lightweight
descriptors instead of binding query code directly to raw state structures.

Current minimal scope
---------------------
This file currently provides:
- a ReadonlyLevelDescriptor containing:
  - level_key
  - record_ids
- a ReadonlyInspectionDescriptor containing:
  - stored_records
  - pending_records
  - level_partition_count
  - listener_count
- a level resolver
- a module inspection resolver

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- projection-target descriptors
- readonly pagination hints
- query/filter descriptors
- snapshot material references
- richer inspection/read-model descriptors
- report-oriented shaping helpers

What should NOT live here
-------------------------
This file must NOT:
- format preview output
- execute queries
- mutate state
- interpret payload business semantics
- become a reporting engine
- become a projection renderer

Design rule
-----------
This file computes readonly reference/view metadata only.
It shapes access descriptors but does not render or execute higher-level views.
------------------------------------------------------------------------------
*/

#include <string>
#include <vector>

#include "logging_system/H_State/log_container_module.hpp"

namespace logging_system::E_Resolvers {

struct ReadonlyLevelDescriptor final {
    std::string level_key{};
    std::vector<std::string> record_ids{};
};

struct ReadonlyInspectionDescriptor final {
    std::size_t stored_records{0};
    std::size_t pending_records{0};
    std::size_t level_partition_count{0};
    std::size_t listener_count{0};
};

struct DefaultReadonlyResolver final {
    template <typename TRecord>
    static ReadonlyLevelDescriptor resolve_level(
        const logging_system::H_State::LogContainerModule<TRecord>& module,
        const std::string& level_key) {
        return ReadonlyLevelDescriptor{
            level_key,
            module.level_containers().list_records(level_key)
        };
    }

    template <typename TRecord>
    static ReadonlyInspectionDescriptor resolve_inspection(
        const logging_system::H_State::LogContainerModule<TRecord>& module) {
        const auto inspection = module.inspect();

        return ReadonlyInspectionDescriptor{
            inspection.stored_records,
            inspection.pending_records,
            inspection.level_partition_count,
            inspection.listener_count
        };
    }
};

}  // namespace logging_system::E_Resolvers