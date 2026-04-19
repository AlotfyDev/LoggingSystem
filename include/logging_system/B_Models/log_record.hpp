#pragma once

#include <optional>
#include <string>
#include <utility>

namespace logging_system::B_Models {

template <typename TPayload>
struct LogRecord final {
    using PayloadType = TPayload;

    std::string record_id{};
    TPayload payload{};
    std::string created_at_utc{};
    std::optional<std::string> dispatched_at_utc{std::nullopt};
    std::optional<std::string> adapter_key{std::nullopt};

    LogRecord() = default;

    LogRecord(
        std::string record_id_in,
        TPayload payload_in,
        std::string created_at_utc_in,
        std::optional<std::string> dispatched_at_utc_in = std::nullopt,
        std::optional<std::string> adapter_key_in = std::nullopt)
        : record_id(std::move(record_id_in)),
          payload(std::move(payload_in)),
          created_at_utc(std::move(created_at_utc_in)),
          dispatched_at_utc(std::move(dispatched_at_utc_in)),
          adapter_key(std::move(adapter_key_in)) {}
};

}  // namespace logging_system::B_Models