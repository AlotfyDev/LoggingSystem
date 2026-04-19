#pragma once

#include <string>
#include <utility>

namespace logging_system::D_Preparation {

struct DefaultContentSchemaApplier final {
    template <typename TEnvelope>
    static TEnvelope apply(
        TEnvelope envelope,
        const std::string& schema_id) {
        envelope.content_schema_id = schema_id;
        return envelope;
    }

    template <typename TEnvelope, typename TSchemaCatalog>
    static TEnvelope apply(
        TEnvelope envelope,
        const std::string& schema_id,
        const TSchemaCatalog&) {
        envelope.content_schema_id = schema_id;
        return envelope;
    }
};

}  // namespace logging_system::D_Preparation