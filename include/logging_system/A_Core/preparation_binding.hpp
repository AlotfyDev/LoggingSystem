#pragma once

#include "logging_system/A_Core/binding_tags.hpp"

namespace logging_system::A_Core {

template <
    typename TMetadataInjector,
    typename TTimestampStabilizer,
    typename TSchemaApplier,
    typename TEnvelopeAssembler,
    typename TRecordStabilizer>
struct PreparationBinding final {
    using binding_tag = preparation_binding_tag;

    using MetadataInjector = TMetadataInjector;
    using TimestampStabilizer = TTimestampStabilizer;
    using SchemaApplier = TSchemaApplier;
    using EnvelopeAssembler = TEnvelopeAssembler;
    using RecordStabilizer = TRecordStabilizer;
};

}  // namespace logging_system::A_Core