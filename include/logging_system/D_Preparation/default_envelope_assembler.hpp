#pragma once

/*
------------------------------------------------------------------------------
D_Preparation/default_envelope_assembler.hpp

Reference Version
-----------------
CONSUMING_PIPELINES_CONSISTENCY_FIX_V1

Role in the architecture
------------------------
This file defines the minimal envelope assembler model for consuming pipelines.

It answers narrow questions such as:
    "What is the preparation component directly bound to a level API façade?"
    "How can plain and validating assemblers share common binding context
     without collapsing into one monolithic implementation?"
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
- metadata is administratively assigned and should be provided through a
  metadata injector/provider path
- the envelope carries content, metadata, and content-update timestamp
- plain and validating assembly should be separate variants
- activation of validating vs plain behavior is a configuration concern, not an
  API concern
- schema/config/binding details must live inside the assembler, not in the API
  call

The later consistency review additionally clarified that:
- timestamp authority should live in the envelope, not in the assembler
- metadata authority should live in the injector/provider, not in a duplicated
  raw assembler field

This means the assembler layer must provide:
- a common base assembler model
- bound façade/API identity info
- internal schema/content binding context
- a metadata injector/provider dependency
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
- `EnvelopeAssemblerBase<TDerived, TEnvelope, TTimestampStabilizer, TMetadataInjector>`
- `PlainEnvelopeAssembler<TEnvelope, TTimestampStabilizer, TMetadataInjector>`
- `ValidatingEnvelopeAssembler<TEnvelope, TTimestampStabilizer, TMetadataInjector>`

The current model is intentionally minimal:
- plain assembler accepts content and materializes/updates the envelope
- validating assembler keeps the same public API shape, but reserves an
  architecturally distinct slot for future internal contract enforcement
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

Timestamp authority remains with the envelope.
Metadata authority remains with the metadata injector/provider path.

This file uses CRTP for shared assembler behavior instead of runtime virtual
dispatch.
------------------------------------------------------------------------------
*/

#include <string>
#include <utility>

#include "logging_system/D_Preparation/default_metadata_injector.hpp"
#include "logging_system/D_Preparation/timestamp_stabilizer.hpp"

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

template <
    typename TDerived,
    typename TEnvelope,
    typename TTimestampStabilizer = UtcEpochMillisStabilizer,
    typename TMetadataInjector = DefaultMetadataInjector>
class EnvelopeAssemblerBase {
public:
    using EnvelopeType = TEnvelope;
    using WrappedContentType = typename TEnvelope::WrappedContentType;
    using TimestampStabilizerType = TTimestampStabilizer;
    using MetadataInjectorType = TMetadataInjector;
    using MetadataType = typename TMetadataInjector::MetadataType;

    EnvelopeAssemblerBase() = default;

    EnvelopeAssemblerBase(
        ApiFacadeBindingInfo binding_info_in,
        TMetadataInjector metadata_injector_in = {},
        std::string content_schema_id_in = {},
        TTimestampStabilizer timestamp_stabilizer_in = {})
        : binding_info_(std::move(binding_info_in)),
          metadata_injector_(std::move(metadata_injector_in)),
          content_schema_id_(std::move(content_schema_id_in)),
          timestamp_stabilizer_(std::move(timestamp_stabilizer_in)) {}

    [[nodiscard]] const ApiFacadeBindingInfo& binding_info() const noexcept {
        return binding_info_;
    }

    [[nodiscard]] const TMetadataInjector& metadata_injector() const noexcept {
        return metadata_injector_;
    }

    [[nodiscard]] const std::string& content_schema_id() const noexcept {
        return content_schema_id_;
    }

    [[nodiscard]] const TTimestampStabilizer& timestamp_stabilizer() const noexcept {
        return timestamp_stabilizer_;
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
        return TEnvelope{
            std::move(content),
            metadata_injector_.get_metadata(),
            content_schema_id_
        };
    }

    void assign_content_to_envelope(
        TEnvelope& envelope,
        WrappedContentType content) const {
        envelope.assign_content(std::move(content));
        envelope.assign_metadata(metadata_injector_.get_metadata());
        envelope.assign_content_schema_id(content_schema_id_);
    }

private:
    [[nodiscard]] const TDerived& derived() const noexcept {
        return static_cast<const TDerived&>(*this);
    }

    ApiFacadeBindingInfo binding_info_{};
    TMetadataInjector metadata_injector_{};
    std::string content_schema_id_{};
    TTimestampStabilizer timestamp_stabilizer_{};
};

template <
    typename TEnvelope,
    typename TTimestampStabilizer = UtcEpochMillisStabilizer,
    typename TMetadataInjector = DefaultMetadataInjector>
class PlainEnvelopeAssembler final
    : public EnvelopeAssemblerBase<
          PlainEnvelopeAssembler<TEnvelope, TTimestampStabilizer, TMetadataInjector>,
          TEnvelope,
          TTimestampStabilizer,
          TMetadataInjector> {
public:
    using Base = EnvelopeAssemblerBase<
        PlainEnvelopeAssembler<TEnvelope, TTimestampStabilizer, TMetadataInjector>,
        TEnvelope,
        TTimestampStabilizer,
        TMetadataInjector>;

    using WrappedContentType = typename Base::WrappedContentType;

    using Base::Base;

    [[nodiscard]] static PlainEnvelopeAssembler CreatePlain(
        ApiFacadeBindingInfo binding_info,
        TMetadataInjector metadata_injector = {},
        std::string content_schema_id = {},
        TTimestampStabilizer timestamp_stabilizer = {}) {
        return PlainEnvelopeAssembler{
            std::move(binding_info),
            std::move(metadata_injector),
            std::move(content_schema_id),
            std::move(timestamp_stabilizer)
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

template <
    typename TEnvelope,
    typename TTimestampStabilizer = UtcEpochMillisStabilizer,
    typename TMetadataInjector = DefaultMetadataInjector>
class ValidatingEnvelopeAssembler final
    : public EnvelopeAssemblerBase<
          ValidatingEnvelopeAssembler<TEnvelope, TTimestampStabilizer, TMetadataInjector>,
          TEnvelope,
          TTimestampStabilizer,
          TMetadataInjector> {
public:
    using Base = EnvelopeAssemblerBase<
        ValidatingEnvelopeAssembler<TEnvelope, TTimestampStabilizer, TMetadataInjector>,
        TEnvelope,
        TTimestampStabilizer,
        TMetadataInjector>;

    using WrappedContentType = typename Base::WrappedContentType;

    using Base::Base;

    [[nodiscard]] static ValidatingEnvelopeAssembler CreateValidating(
        ApiFacadeBindingInfo binding_info,
        TMetadataInjector metadata_injector = {},
        std::string content_schema_id = {},
        TTimestampStabilizer timestamp_stabilizer = {}) {
        return ValidatingEnvelopeAssembler{
            std::move(binding_info),
            std::move(metadata_injector),
            std::move(content_schema_id),
            std::move(timestamp_stabilizer)
        };
    }

    [[nodiscard]] TEnvelope accept_content_impl(
        WrappedContentType content) const {
        // Validation-aware specialization is intentionally deferred.
        // The validating variant keeps the same public API shape while
        // reserving an architecturally distinct internal path for future
        // contract enforcement.
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

private:
    using Base::Base;
};

}  // namespace logging_system::D_Preparation