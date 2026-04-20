#pragma once

/*
------------------------------------------------------------------------------
N_System/profile_registry.hpp

Reference Version
-----------------
SYSTEM_GOVERNANCE_P1_BASELINE_V1

Role in the architecture
------------------------
ProfileRegistry is the minimal typed access surface for runtime and production
profile identity within the composed system.

It answers narrow questions such as:
    "What production profile is currently active?"
    "How can the system root keep profile identity visible without embedding
     full governance CRUD logic into the root itself?"

Why this file exists in this stage
----------------------------------
The Python architecture distinguishes between:
- profile definition/catalog governance
- profile activation/application into runtime bindings

This file captures the minimal profile-facing part that the system root needs:
- active profile identity
- available profile identifiers

without collapsing profile governance into the system root.

Current minimal scope
---------------------
This file currently provides:
- `ProfileRegistry`
- storage for:
  - active production profile id
  - known runtime profile ids
  - known production profile ids
- minimal read/write helpers for these identities

This is intentionally identity-focused and minimal.
It is not a full governance service.

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- richer profile descriptors
- validation hooks
- active/inactive state metadata
- compatibility checks against pipeline catalog
- bindings to production profile services/catalogs

What should NOT live here
-------------------------
This file must NOT:
- implement full profile CRUD policy
- own provider catalogs
- own configuration mappers/validators
- activate adapters or runtime bindings directly
- become a governance monolith

Design rule
-----------
This file is a minimal profile-identity registry for composed system use.
It keeps active/known profile identity visible without swallowing the
governance layer.
------------------------------------------------------------------------------
*/

#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace logging_system::N_System {

struct ProfileRegistry final {
    std::optional<std::string> active_production_profile_id{std::nullopt};
    std::vector<std::string> runtime_profile_ids{};
    std::vector<std::string> production_profile_ids{};

    void set_active_production_profile(std::string profile_id) {
        active_production_profile_id = std::move(profile_id);
    }

    void clear_active_production_profile() {
        active_production_profile_id.reset();
    }

    [[nodiscard]] const std::optional<std::string>&
    get_active_production_profile() const noexcept {
        return active_production_profile_id;
    }

    [[nodiscard]] const std::vector<std::string>&
    list_runtime_profiles() const noexcept {
        return runtime_profile_ids;
    }

    [[nodiscard]] const std::vector<std::string>&
    list_production_profiles() const noexcept {
        return production_profile_ids;
    }
};

}  // namespace logging_system::N_System