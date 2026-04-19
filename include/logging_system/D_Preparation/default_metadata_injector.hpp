#pragma once

#include <utility>

namespace logging_system::D_Preparation {

struct DefaultMetadataInjector final {
    template <typename TMetadata>
    static TMetadata inject(TMetadata metadata) {
        return metadata;
    }

    template <typename TMetadata, typename TEnvelope>
    static TEnvelope inject_into(TEnvelope envelope, TMetadata metadata) {
        envelope.metadata = std::move(metadata);
        return envelope;
    }
};

}  // namespace logging_system::D_Preparation