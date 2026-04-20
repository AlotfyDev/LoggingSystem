#pragma once

/*
------------------------------------------------------------------------------
D_Preparation/default_metadata_injector.hpp

Reference Version
-----------------
CONSUMING_PIPELINES_CORRECTION_BASELINE_V1

Role in the architecture
------------------------
This file defines the default metadata injector used by consuming-pipeline
assemblers.

It answers narrow questions such as:
    "What is the default preparation-side metadata injector for the logging
     system?"
    "How is administratively assigned log metadata made available to the
     assembler by default?"
    "How can the system provide a minimal default injector without leaking
     metadata concerns into consumer-facing APIs?"

Why this file exists in this stage
----------------------------------
The consuming-pipeline correction clarified that:
- the consumer-facing API should only pass content
- metadata should be assigned by higher administrative/managerial layers
- assembler composition should include a metadata injector dependency
- the vertical metadata path should be closed before duplicating corrected API
  logic across all level façades

The project already has:
- a metadata value model in `B_Models/log_metadata.hpp`
- a generic injector model in `D_Preparation/metadata_injector.hpp`
- a concept-like contract vocabulary in
  `C_Contracts/metadata_injector_concept.hpp`

This file closes the default path by binding those pieces together into a
default injector.

Current minimal scope
---------------------
This file currently provides:
- `DefaultMetadataInjector`

The default injector:
- uses `logging_system::B_Models::LogMetadata`
- inherits the generic injector behavior
- supports constructor-based metadata assignment
- exposes `get_metadata()` and `set_metadata(...)`

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- default administrative seeding helpers
- immutable default variants
- richer metadata refresh/update policies
- compatibility checks against assembler families

What should NOT live here
-------------------------
This file must NOT:
- validate content
- assemble envelopes
- manage timestamps
- manage slot/record state
- become an administrative service

Design rule
-----------
This file defines the default metadata injector only.
It provides the default preparation-side metadata dependency that assemblers
compose internally.
------------------------------------------------------------------------------
*/

#include "logging_system/B_Models/log_metadata.hpp"
#include "logging_system/D_Preparation/metadata_injector.hpp"

namespace logging_system::D_Preparation {

class DefaultMetadataInjector final
    : public MetadataInjector<logging_system::B_Models::LogMetadata> {
public:
    using Base = MetadataInjector<logging_system::B_Models::LogMetadata>;
    using MetadataType = typename Base::MetadataType;

    DefaultMetadataInjector() = default;

    explicit DefaultMetadataInjector(MetadataType metadata_in)
        : Base(std::move(metadata_in)) {}

    // Override inject_into for any specialized default behavior
    template <typename TEnvelope>
    void inject_into(TEnvelope& envelope, const MetadataType& metadata) const {
        // Default implementation - just assign metadata
        envelope.assign_metadata(metadata);
    }
};

}  // namespace logging_system::D_Preparation