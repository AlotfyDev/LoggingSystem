#pragma once

/*
------------------------------------------------------------------------------
D_Preparation/metadata_injector.hpp

Reference Version
-----------------
CONSUMING_PIPELINES_CORRECTION_BASELINE_V1

Role in the architecture
------------------------
This file defines the generic metadata injector base model for consuming
pipeline preparation.

It answers narrow questions such as:
    "What is the reusable preparation-side object that carries administratively
     assigned metadata into assemblers?"
    "How can metadata provision remain isolated from consumer-facing APIs while
     still being explicitly modeled?"
    "What is the smallest reusable injector abstraction needed before building
     concrete default injectors?"

Why this file exists in this stage
----------------------------------
The consuming-pipeline correction clarified that:
- metadata belongs to preparation, not to the consumer-facing API
- metadata is assigned by a higher layer and then composed into the assembler
- the consumer should never pass metadata manually on each call
- the assembler should rely on a preparation-side dependency for metadata

This means the project needs a generic metadata injector model that:
- owns or carries a metadata object
- exposes it through a stable preparation-facing method
- remains independent from:
  - API façade logic
  - envelope assembly logic
  - registry/state logic

Current minimal scope
---------------------
This file currently provides:
- `MetadataInjector<TMetadata>`

The generic injector currently provides:
- `MetadataType`
- constructor-based metadata assignment
- `get_metadata() const`
- `set_metadata(...)`

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- immutable-only injector variants
- metadata refresh hooks
- injector specializations per administrative/runtime profile
- policy-bound injector families

What should NOT live here
-------------------------
This file must NOT:
- validate content
- assemble envelopes
- manage record/slot state
- expose administrative UI logic
- become the metadata model itself

Design rule
-----------
This file defines the generic preparation-side metadata injector object only.
It carries metadata for assembler composition and exposes it through a stable
injector-facing API.
------------------------------------------------------------------------------
*/

#include <utility>

namespace logging_system::D_Preparation {

// Forward declaration for envelope types
template <typename TWrappedContent, typename TMetadata>
struct LogEnvelope;

template <typename TMetadata>
class MetadataInjector {
public:
    using MetadataType = TMetadata;
    // Note: EnvelopeType is defined by derived classes or template specializations
    // This allows different injectors to work with different envelope types

    MetadataInjector() = default;

    explicit MetadataInjector(TMetadata metadata_in)
        : metadata_(std::move(metadata_in)) {}

    [[nodiscard]] const TMetadata& get_metadata() const noexcept {
        return metadata_;
    }

    void set_metadata(TMetadata metadata_in) {
        metadata_ = std::move(metadata_in);
    }

    // Default injection implementation - derived classes can override for specialized behavior
    template <typename TEnvelope>
    void inject_into(TEnvelope& envelope, const TMetadata& metadata) const {
        envelope.assign_metadata(metadata);
    }

    // Convenience overload that uses internal metadata
    template <typename TEnvelope>
    void inject_into(TEnvelope& envelope) const {
        inject_into(envelope, metadata_);
    }

protected:
    TMetadata metadata_{};
};

}  // namespace logging_system::D_Preparation