#pragma once

#include <string>
#include <utility>

#include "logging_system/B_Models/log_envelope.hpp"

namespace logging_system::D_Preparation {

struct DefaultEnvelopeAssembler final {
    template <typename TContent, typename TContext, typename TMetadata>
    static auto assemble(
        TContent content,
        TContext context,
        TMetadata metadata,
        std::string created_at_utc,
        std::string content_schema_id) {
        using Envelope = logging_system::B_Models::LogEnvelope<
            std::decay_t<TContent>,
            std::decay_t<TContext>,
            std::decay_t<TMetadata>>;

        return Envelope{
            std::move(content),
            std::move(context),
            std::move(metadata),
            std::move(created_at_utc),
            std::move(content_schema_id)};
    }
};

}  // namespace logging_system::D_Preparation