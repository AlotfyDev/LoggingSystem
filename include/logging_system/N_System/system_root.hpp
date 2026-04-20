#pragma once

/*
------------------------------------------------------------------------------
N_System/system_root.hpp

Reference Version
-----------------
SYSTEM_GOVERNANCE_P1_BASELINE_V1

Role in the architecture
------------------------
SystemRoot is the composition root of the logging system.

It answers narrow questions such as:
    "Which major subsystems are bound together in this system build?"
    "How are pipeline catalog, profile identity, state, adapters, governance,
     and user-facing surfaces made available without recreating a monolithic
     runtime service object?"

Why this file exists in this stage
----------------------------------
The Python architecture currently uses a central runtime service object as a
convergence root, but the intended C++ architecture deliberately avoids
recreating that shape.

This file therefore exists to provide:
- composition ownership
- subsystem visibility
- surface access

without:
- central ingest convergence
- generic shared level logging dispatch
- governance/runtime monolith behavior

Current minimal scope
---------------------
This file currently provides:
- `SystemRoot<TStateCore, TGovernance, TAdapterBoundary, TSurfaces>`
- ownership of:
  - state core
  - governance bundle
  - adapter boundary
  - profile registry
- compile-time visibility of:
  - `PipelineCatalog`
- access to the bound surfaces object

This is intentionally a composition shell.
It does not implement runtime orchestration behavior itself.

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- builder integration
- activation helpers
- configuration application bridge hooks
- profile-to-runtime binding hooks
- validation against pipeline/profile compatibility
- optional construction presets

What should NOT live here
-------------------------
This file must NOT:
- become a shared `log(level=...)` service
- implement handler logic
- implement dispatch execution
- implement governance CRUD
- replace dedicated level APIs or specialized pipelines

Design rule
-----------
This file is the system composition root only.
It binds major subsystems together and exposes them, while preserving the
decomposed pipeline-centered architecture.
------------------------------------------------------------------------------
*/

#include <utility>

#include "logging_system/N_System/pipeline_catalog.hpp"
#include "logging_system/N_System/profile_registry.hpp"

namespace logging_system::N_System {

template <
    typename TStateCore,
    typename TGovernance,
    typename TAdapterBoundary,
    typename TSurfaces>
struct SystemRoot final {
    using PipelinePack = PipelineCatalog;
    using ProfileRegistryType = logging_system::N_System::ProfileRegistry;

    TStateCore state_core;
    TGovernance governance;
    TAdapterBoundary adapter_boundary;
    ProfileRegistryType profile_registry;
    TSurfaces surfaces;

    SystemRoot(
        TStateCore state_core_in,
        TGovernance governance_in,
        TAdapterBoundary adapter_boundary_in,
        TSurfaces surfaces_in,
        ProfileRegistryType profile_registry_in = {})
        : state_core(std::move(state_core_in)),
          governance(std::move(governance_in)),
          adapter_boundary(std::move(adapter_boundary_in)),
          profile_registry(std::move(profile_registry_in)),
          surfaces(std::move(surfaces_in)) {}

    [[nodiscard]] TSurfaces& expose_surfaces() noexcept {
        return surfaces;
    }

    [[nodiscard]] const TSurfaces& expose_surfaces() const noexcept {
        return surfaces;
    }

    [[nodiscard]] TStateCore& expose_state_core() noexcept {
        return state_core;
    }

    [[nodiscard]] const TStateCore& expose_state_core() const noexcept {
        return state_core;
    }

    [[nodiscard]] TGovernance& expose_governance() noexcept {
        return governance;
    }

    [[nodiscard]] const TGovernance& expose_governance() const noexcept {
        return governance;
    }

    [[nodiscard]] TAdapterBoundary& expose_adapter_boundary() noexcept {
        return adapter_boundary;
    }

    [[nodiscard]] const TAdapterBoundary& expose_adapter_boundary() const noexcept {
        return adapter_boundary;
    }

    [[nodiscard]] ProfileRegistryType& expose_profile_registry() noexcept {
        return profile_registry;
    }

    [[nodiscard]] const ProfileRegistryType& expose_profile_registry() const noexcept {
        return profile_registry;
    }
};

}  // namespace logging_system::N_System