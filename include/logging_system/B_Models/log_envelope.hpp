#pragma once

#include <string>
#include <utility>

namespace logging_system::B_Models {

template <typename TContent, typename TContext, typename TMetadata>
struct LogEnvelope final {
    using ContentType = TContent;
    using ContextType = TContext;
    using MetadataType = TMetadata;

    TContent content{};
    TContext context{};
    TMetadata metadata{};

    std::string created_at_utc{};
    std::string content_schema_id{};

    LogEnvelope() = default;

    LogEnvelope(
        TContent content_in,
        TContext context_in,
        TMetadata metadata_in,
        std::string created_at_utc_in,
        std::string content_schema_id_in)
        : content(std::move(content_in)),
          context(std::move(context_in)),
          metadata(std::move(metadata_in)),
          created_at_utc(std::move(created_at_utc_in)),
          content_schema_id(std::move(content_schema_id_in)) {}
};

}  // namespace logging_system::B_Models