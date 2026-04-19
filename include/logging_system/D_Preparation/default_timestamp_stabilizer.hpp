#pragma once

#include <string>

#include "logging_system/B_Models/utc_now_iso.hpp"

namespace logging_system::D_Preparation {

struct DefaultTimestampStabilizer final {
    static std::string stabilize() {
        return logging_system::B_Models::utc_now_iso();
    }

    template <typename TEnvelope>
    static TEnvelope inject_into(TEnvelope envelope) {
        envelope.created_at_utc = logging_system::B_Models::utc_now_iso();
        return envelope;
    }
};

}  // namespace logging_system::D_Preparation