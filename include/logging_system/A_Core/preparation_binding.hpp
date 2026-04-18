#pragma once

#include <type_traits>

namespace logging_system::A_Core {

template <
    typename TMetadataInjector,
    typename TTimestampStabilizer,
    typename TSchemaApplier,
    typename TEnvelopeAssembler,
    typename TRecordStabilizer>
struct PreparationBinding final {
    using MetadataInjector = TMetadataInjector;
    using TimestampStabilizer = TTimestampStabilizer;
    using SchemaApplier = TSchemaApplier;
    using EnvelopeAssembler = TEnvelopeAssembler;
    using RecordStabilizer = TRecordStabilizer;

    static constexpr bool is_preparation_binding = true;
};

template <typename T>
inline constexpr bool is_preparation_binding_v =
    std::remove_cv_t<std::remove_reference_t<T>>::is_preparation_binding;

}  // namespace logging_system::A_Core