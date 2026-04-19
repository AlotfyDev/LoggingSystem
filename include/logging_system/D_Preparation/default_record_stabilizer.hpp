#pragma once

#include <optional>
#include <string>
#include <utility>

#include "logging_system/B_Models/log_record.hpp"

namespace logging_system::D_Preparation {

struct DefaultRecordStabilizer final {
    template <typename TPayload>
    static auto stabilize(
        std::string record_id,
        TPayload payload,
        std::string created_at_utc,
        std::optional<std::string> dispatched_at_utc = std::nullopt,
        std::optional<std::string> adapter_key = std::nullopt) {
        using Record = logging_system::B_Models::LogRecord<std::decay_t<TPayload>>;

        return Record{
            std::move(record_id),
            std::move(payload),
            std::move(created_at_utc),
            std::move(dispatched_at_utc),
            std::move(adapter_key)};
    }

    template <typename TEnvelope, typename TPayloadBuilder>
    static auto stabilize_from_envelope(
        const std::string& record_id,
        const TEnvelope& envelope,
        TPayloadBuilder&& payload_builder,
        std::optional<std::string> dispatched_at_utc = std::nullopt,
        std::optional<std::string> adapter_key = std::nullopt) {
        return stabilize(
            record_id,
            payload_builder(envelope),
            envelope.created_at_utc,
            std::move(dispatched_at_utc),
            std::move(adapter_key));
    }
};

}  // namespace logging_system::D_Preparation