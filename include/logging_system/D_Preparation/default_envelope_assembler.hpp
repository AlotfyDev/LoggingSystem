#pragma once

/*
------------------------------------------------------------------------------
D_Preparation/default_envelope_assembler.hpp

Reference Version
-----------------
CONSUMING_PIPELINES_CORRECTION_BASELINE_V2

Role in the architecture
------------------------
This file defines the minimal envelope assembler model for consuming pipelines.

It answers narrow questions such as:
    "What is the preparation component directly bound to a level API façade?"
    "How can plain and validating assemblers share common metadata and binding
     context without collapsing into one monolithic implementation?"
    "What should the API know about the assembler, and what should remain
     hidden behind the assembler boundary?"

Why this file exists in this stage
----------------------------------
The corrected consuming-pipeline model clarified that:
- the level API is a thin, user-visible façade
- the API is independent as a surface object
- the assembler is owned by the API through composition
- the user sees only the API
- the consumer-facing path must expose content only
- metadata is administratively assigned and stored in the assembler
- the envelope carries content, metadata, and content-update timestamp
- plain and validating assembly should be separate variants
- activation of validating vs plain behavior is a configuration concern, not an
  API concern
- schema/config/binding details must live inside the assembler, not in the API
  call

This means the assembler layer must provide:
- a common base assembler model
- bound metadata object
- bound façade/API identity info
- optional schema/content binding context held internally
- a standard `accept_content(...)` entry shape
- plain and validating specialized descendants
- static factory methods
- no registry logic
- no dispatch logic
- no slot-state management

Current minimal scope
---------------------
This file currently provides:
- `ApiFacadeBindingInfo`
- `EnvelopeAssemblerBase<TDerived, TEnvelope, TMetadata>`
- `PlainEnvelopeAssembler<TEnvelope, TMetadata>`
- `ValidatingEnvelopeAssembler<TEnvelope, TMetadata>`

The current model is intentionally minimal:
- plain assembler accepts content and materializes/updates the envelope
- validating assembler currently has the same consumer-facing shape, but is
  reserved for future validation-aware specialization
- the API is expected to know only that it can call `accept_content(...)`

What this file should contain in its fuller form later
------------------------------------------------------
Later expansions may include:
- content contract validator dependencies
- richer schema identity resolution hooks
- richer façade binding metadata
- envelope lifecycle hints
- compile-time family constraints
- level-specific specialized descendants/aliases

What should NOT live here
-------------------------
This file must NOT:
- manage registry slots
- manage record state transitions
- dispatch records
- resolve references
- expose governance/admin operations to end users
- become the API façade itself

Design rule
-----------
The assembler is a preparation component owned by a thin API façade.

The API remains an independent user-visible surface.
The assembler remains a hidden owned dependency that accepts content and
materializes/updates the envelope.

This file uses CRTP for shared assembler behavior instead of runtime virtual
dispatch.
------------------------------------------------------------------------------
*/

#include <string>
#include <utility>

#include "logging_system/D_Preparation/timestamp_stabilizer.hpp"
#include "logging_system/D_Preparation/default_metadata_injector.hpp"

namespace logging_system::D_Preparation {

struct ApiFacadeBindingInfo final {
    std::string facade_name{};
    std::string content_family_key{};
    std::string binding_key{};

    ApiFacadeBindingInfo() = default;

    ApiFacadeBindingInfo(
        std::string facade_name_in,
        std::string content_family_key_in,
        std::string binding_key_in)
        : facade_name(std::move(facade_name_in)),
          content_family_key(std::move(content_family_key_in)),
          binding_key(std::move(binding_key_in)) {}
};

template <typename TDerived, typename TEnvelope, typename TMetadata, typename TTimestampStabilizer, typename TMetadataInjector>
class EnvelopeAssemblerBase {
public:
    using EnvelopeType = TEnvelope;
    using MetadataType = TMetadata;
    using WrappedContentType = typename TEnvelope::WrappedContentType;

    EnvelopeAssemblerBase() = default;

    EnvelopeAssemblerBase(
        TMetadata metadata_in,
        ApiFacadeBindingInfo binding_info_in,
        std::string content_schema_id_in = {},
        TTimestampStabilizer timestamp_stabilizer_in = {},
        TMetadataInjector metadata_injector_in = {})
        : metadata_(std::move(metadata_in)),
          binding_info_(std::move(binding_info_in)),
          content_schema_id_(std::move(content_schema_id_in)),
          timestamp_stabilizer_(std::move(timestamp_stabilizer_in)),
          metadata_injector_(std::move(metadata_injector_in)) {}

    [[nodiscard]] const TMetadata& metadata() const noexcept {
        return metadata_;
    }

    [[nodiscard]] const ApiFacadeBindingInfo& binding_info() const noexcept {
        return binding_info_;
    }

    [[nodiscard]] const std::string& content_schema_id() const noexcept {
        return content_schema_id_;
    }

    [[nodiscard]] TEnvelope accept_content(
        WrappedContentType content) const {
        return derived().accept_content_impl(std::move(content));
    }

    void accept_content(
        TEnvelope& envelope,
        WrappedContentType content) const {
        derived().accept_content_impl(
            envelope,
            std::move(content));
    }

protected:
    [[nodiscard]] TEnvelope make_envelope_from_content(
        WrappedContentType content) const {
        TEnvelope envelope{
            std::move(content),
            metadata_,
            content_schema_id_
        };
        metadata_injector_.inject_into(envelope, metadata_);
        timestamp_stabilizer_.stabilize_into(envelope);
        return envelope;
    }

    void assign_content_to_envelope(
        TEnvelope& envelope,
        WrappedContentType content) const {
        envelope.assign_content(std::move(content));
        metadata_injector_.inject_into(envelope, metadata_);
        envelope.assign_content_schema_id(content_schema_id_);
        timestamp_stabilizer_.stabilize_into(envelope);
    }

private:
    [[nodiscard]] const TDerived& derived() const noexcept {
        return static_cast<const TDerived&>(*this);
    }

    TMetadata metadata_{};
    ApiFacadeBindingInfo binding_info_{};
    std::string content_schema_id_{};
    TTimestampStabilizer timestamp_stabilizer_{};
    TMetadataInjector metadata_injector_{};
};

template <typename TEnvelope, typename TMetadata>
class PlainEnvelopeAssembler final
    : public EnvelopeAssemblerBase<
          PlainEnvelopeAssembler<TEnvelope, TMetadata>,
          TEnvelope,
          TMetadata,
          UtcEpochMillisStabilizer,
          DefaultMetadataInjector> {
public:
    using Base = EnvelopeAssemblerBase<
        PlainEnvelopeAssembler<TEnvelope, TMetadata>,
        TEnvelope,
        TMetadata,
        UtcEpochMillisStabilizer,
        DefaultMetadataInjector>;

    using WrappedContentType = typename Base::WrappedContentType;

    using Base::Base;

    [[nodiscard]] static PlainEnvelopeAssembler CreatePlain(
        TMetadata metadata,
        ApiFacadeBindingInfo binding_info,
        std::string content_schema_id = {}) {
        return PlainEnvelopeAssembler{
            std::move(metadata),
            std::move(binding_info),
            std::move(content_schema_id)
        };
    }

    [[nodiscard]] TEnvelope accept_content_impl(
        WrappedContentType content) const {
        return this->make_envelope_from_content(std::move(content));
    }

    void accept_content_impl(
        TEnvelope& envelope,
        WrappedContentType content) const {
        this->assign_content_to_envelope(
            envelope,
            std::move(content));
    }

private:
    using Base::Base;
};

template <typename TEnvelope, typename TMetadata>
class ValidatingEnvelopeAssembler final
    : public EnvelopeAssemblerBase<
          ValidatingEnvelopeAssembler<TEnvelope, TMetadata>,
          TEnvelope,
          TMetadata,
          UtcEpochMillisStabilizer,
          DefaultMetadataInjector> {
public:
    using Base = EnvelopeAssemblerBase<
        ValidatingEnvelopeAssembler<TEnvelope, TMetadata>,
        TEnvelope,
        TMetadata,
        UtcEpochMillisStabilizer,
        DefaultMetadataInjector>;

    using WrappedContentType = typename Base::WrappedContentType;

    using Base::Base;

    [[nodiscard]] static ValidatingEnvelopeAssembler CreateValidating(
        TMetadata metadata,
        ApiFacadeBindingInfo binding_info,
        std::string content_schema_id = {}) {
        return ValidatingEnvelopeAssembler{
            std::move(metadata),
            std::move(binding_info),
            std::move(content_schema_id)
        };
    }

    [[nodiscard]] TEnvelope accept_content_impl(
        WrappedContentType content) const {
        // Validation-aware specialization is intentionally deferred.
        // The validating variant keeps the same consumer-facing shape while
        // reserving the internal path for future contract enforcement.
        return this->make_envelope_from_content(std::move(content));
    }

    void accept_content_impl(
        TEnvelope& envelope,
        WrappedContentType content) const {
        // Validation-aware specialization is intentionally deferred.
        this->assign_content_to_envelope(
            envelope,
            std::move(content));
    }

//private:
//    using Base::Base;
};

}  // namespace logging_system::D_Preparation